import serial
import struct
import time
import re
import binascii
import os
import csv

PORT = "/dev/ttyUSB0"  # Update as needed
BAUD = 115200
PRINT_TELEMETRY_TO_CONSOLE = True

PACKET_HEADER = 0xAA

PACKET_TYPE_TELEMETRY = 0x01
PACKET_TYPE_LOG       = 0x02

HEADER_SIZE           = 11
MAX_MESSAGE_LEN       = 90

TELEMETRY_CSV_PATH = "telemetry.csv"
INFO_ERR_CSV_PATH = "info_error_logs.csv"
INFO_ERR_CSV_FIELDNAMES = [
    "seq", "timestamp",
    "code", "code_str", "message"
]

TELEMETRY_CSV_FIELDNAMES = [
    "seq", "timestamp",
    "acc_x", "acc_y", "acc_z",
    "gyro_x", "gyro_y", "gyro_z",
    "pressure", "altitude",
    "fsm_state", "fsm_state_str",
]

# Mapping code values to strings
MESSAGE_CODES = {
    0:   "INFO_DEBUG",
    1:   "INFO_IMU_INIT_PASS",
    2:   "INFO_BARO_INIT_PASS",
    3:   "INFO_PYRO_ARM_PASS",
    4:   "INFO_FSM_INIT_PASS",
    5:   "INFO_COMPONENT_SANITY_CHECK_PASS",
    6:   "INFO_ENTERED_PREFLIGHT_STAGE",
    7:   "INFO_ENTERED_POWERED_ASCENT_STAGE",
    8:   "INFO_ENTERED_DROGUE_DESCENT_STAGE",
    9:   "INFO_ENTERED_MAIN_DESCENT_STAGE",
    10:  "INFO_DROGUE_PARACHUTE_DEPLOYED",
    11:  "INFO_MAIN_PARACHUTE_DEPLOYED",
    12:  "INFO_DROGUE_PARACHUTE_DEPLOYED_TIMEOUT",
    13:  "INFO_MAIN_PARACHUTE_DEPLOYED_TIMEOUT",
    14:  "INFO_GROUND_PRES_AND_TEMP_SET",
	15:  "INFO_LAUNCH_DETECTED_FROM_ACCELERATION",
	16: "INFO_LAUNCH_DETECTED_FROM_ALTITUDE",
    -1:  "ERR_COMPONENT_SANITY_CHECK_FAIL",
    -2:  "ERR_IMU_INIT_FAIL",
    -3:  "ERR_IMU_CALIB_FAIL",
    -4:  "ERR_IMU_READ_ACCEL_FAIL",
    -5:  "ERR_IMU_READ_GYRO_FAIL",
    -6:  "ERR_BARO_INIT_FAIL",
    -7:  "ERR_BARO_CALIB_FAIL",
    -8:  "ERR_BARO_READ_FAIL",
    -9:  "ERR_PYRO_DROGUE_FAIL",
    -10: "ERR_PYRO_MAIN_FAIL",
    -11: "ERR_TIMEOUT_APOGEE",
    -12: "ERR_I2C_LINE_FAIL",
    -13: "ERR_FSM_STATE_FAIL",
    -14: "ERR_MISC_ERR",
    -15: "ERR_PYRO_ARM_FAIL",
    -16: "ERR_PYRO_SEND_COMMAND_FAIL",
    -17: "ERR_PYRO_RECEIVE_COMMAND_FAIL",
    -18: "ERR_PYRO_BACKUP_FAIL",
}

FSM_STATE = {
    0: "FLIGHT_STATE_PREFLIGHT",
    1: "FLIGHT_STATE_POWERED_ASCENT",
    2: "FLIGHT_STATE_DROGUE_DESCENT",
    3: "FLIGHT_STATE_MAIN_DESCENT",
    4: "FLIGHT_STATE_ERROR",
}

def calc_checksum(data: bytes) -> int:
    """XOR checksum matching sender C code."""
    crc = 0
    for b in data[1:-1]:  #skip header, include everything except CRC
        crc ^= b
    return crc & 0xFF


def parse_packet(buf: bytes):
    try:
        header, seq, length, timestamp, pkt_type = struct.unpack_from(">BIBIB", buf, 0)
    except struct.error as e:
        print(f"[WARN] packet header unpack failed: {e}")
        return None

    if (header != PACKET_HEADER):
        print(f"[WARN] Packet header magic number mismatch: expected {PACKET_HEADER}, got {header}")
        return None

    if (length != len(buf)):
        print(f"[WARN] Length mismatch: expected {length}, got {len(buf)}")
        return None

    crc_calc = calc_checksum(buf)
    crc_recv = buf[-1]
    if crc_calc != crc_recv:
        print(f"[WARN] CRC mismatch: expected {crc_calc:02X}, got {crc_recv:02X}")
        print(f"[DEBUG] Raw packet: {buf.hex(' ')}")
        return None

    offset = HEADER_SIZE

    if pkt_type == PACKET_TYPE_TELEMETRY:
        try:
            acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, pressure, altitude, fsm_state = \
                struct.unpack_from(">6hihB", buf, offset)

        except struct.error as e:
            print(f"[WARN] telemetry unpack failed: {e}")
            return None

        return ("telemetry", {
            "seq": seq,
            "timestamp": timestamp,
            "acc_x": acc_x, "acc_y": acc_y, "acc_z": acc_z,
            "gyro_x": gyro_x, "gyro_y": gyro_y, "gyro_z": gyro_z,
            "pressure": pressure,
            "altitude": altitude / 100,
            "fsm_state": fsm_state,
            "fsm_state_str": FSM_STATE.get(fsm_state, f"UNKNOWN({fsm_state})"),
        })

    elif pkt_type == PACKET_TYPE_LOG:
        code, msg_len = struct.unpack_from(">bB", buf, offset)
        offset += 2

        if msg_len > 0:
            message_bytes = buf[offset:offset+msg_len]
            message = message_bytes.decode(errors="replace")
        else:
            message = None

        return ("log", {
            "seq": seq,
            "timestamp": timestamp,
            "code": code,
            "code_str": MESSAGE_CODES.get(code, f"UNKNOWN({code})"),
            "message": message
        })

    return None


def send_at_command(ser, cmd, wait=1.0):
    ser.reset_input_buffer()
    ser.write((cmd + "\r\n").encode())
    time.sleep(wait)
    resp = ser.read_all().decode(errors="ignore").strip()
    print(f">> {cmd}")
    print(f"<< {resp if resp else '(no response)'}")
    return "OK" in resp or "TEST" in resp or "RXLRPKT" in resp


def main():
    print(f"Opening {PORT} @ {BAUD}...")
    ser = serial.Serial(PORT, BAUD, timeout=0.05)  # shorter timeout
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    time.sleep(2)

    print("\nConfiguring module with AT commands...")
    send_at_command(ser, "AT")
    send_at_command(ser, "AT+MODE=TEST")
    send_at_command(ser, "AT+TEST=RFCFG,868,7,500,8,12,14,ON,OFF,OFF")
    send_at_command(ser, "AT+TEST=RXLRPKT")

    print("\nModule configured. Listening for packets...\n")

    regex_rx = re.compile(r'\+TEST:\s*RX\s*"([0-9A-Fa-f]+)"')
    buffer = ""

    telemetry_file = open(TELEMETRY_CSV_PATH, "a", newline="")
    writer = csv.writer(telemetry_file)

    try:
        if os.fstat(telemetry_file.fileno()).st_size == 0:
            writer.writerow(TELEMETRY_CSV_FIELDNAMES)
    except Exception:
        print("[WARN] Failed to write csv header")

    try:
        while True:
            data = ser.read(256).decode(errors="ignore")
            if not data:
                continue

            buffer += data

            # Process all lines we can find
            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                line = line.strip()
                if not line:
                    continue

                match = regex_rx.search(line)
                if match:
                    hex_payload = match.group(1)
                    try:
                        packet_bytes = binascii.unhexlify(hex_payload)

                        parsed = parse_packet(packet_bytes)
                        if parsed:
                            pkt_type, d = parsed
                            if pkt_type == "telemetry":
                                if PRINT_TELEMETRY_TO_CONSOLE:
                                    print(f"[TLM] seq={d['seq']} ts={d['timestamp']}")
                                    print(f"      acc=({d['acc_x']},{d['acc_y']},{d['acc_z']})")
                                    print(f"      gyro=({d['gyro_x']},{d['gyro_y']},{d['gyro_z']})")
                                    print(f"      pres={d['pressure']} alt={d['altitude']}")
                                    print(f"      state={d['fsm_state_str']}")

                                writer.writerow(d.values())
                            elif pkt_type == "log":
                                log_type = "INFO" if d['code'] > 0 else "ERROR"
                                print(f"[{log_type}] seq={d['seq']} ts={d['timestamp']} code={d['code_str']}")
                                if d['message']:
                                    print(f"        msg: {d['message']}")

                    except Exception as e:
                        print(f"[ERROR] Failed to parse RX hex: {e}")
    except KeyboardInterrupt:
        telemetry_file.close()
        print("\nStopped.")


if __name__ == "__main__":
    main()

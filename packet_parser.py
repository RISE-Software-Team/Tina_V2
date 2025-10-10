import serial
import struct
import time
import re
import binascii
PORT = "/dev/ttyUSB1"
BAUD = 9600

PACKET_HEADER = 0xAA
PACKET_TYPE_TELEMETRY = 0x01
PACKET_TYPE_INFO = 0x02
PACKET_TYPE_ERROR = 0x03
PACKET_TYPE_DEBUG = 0x04
MAX_MESSAGE_LEN = 90


ERROR_CODES = {
    0x01: "ERR_IMU_FAIL",
    0x02: "ERR_IMU_INIT_FAIL",
    0x03: "ERR_IMU_CALIB_FAIL",
    0x04: "ERR_BARO_FAIL",
    0x05: "ERR_BARO_INIT_FAIL",
    0x06: "ERR_BARO_CALIB_FAIL",
    0x07: "ERR_PYRO_DROGUE_FAIL",
    0x08: "ERR_PYRO_MAIN_FAIL",
    0x09: "ERR_TIMEOUT_APOGEE",
    0x0A: "ERR_MISC_ERR",
    0x0B: "ERR_LOGIC_FAIL",
    0x0C: "ERR_COMPONENT_SANITY_CHECK_FAIL",
}

INFO_CODES = {
    0x01: "INFO_ENTERED_PREFLIGHT_STAGE",
    0x02: "INFO_ENTERED_POWERED_ASCENT_STAGE",
    0x03: "INFO_ENTERED_DROGUE_DESCENT_STAGE",
    0x04: "INFO_ENTERED_MAIN_DESCENT_STAGE",
    0x05: "INFO_ENTERED_TOUCHDOWN_STAGE",
    0x06: "INFO_DROGUE_PARACHUTE_DEPLOYED",
    0x07: "INFO_MAIN_PARACHUTE_DEPLOYED",
    0x08: "INFO_COMPONENT_SANITY_CHECK_PASS",
}

SEVERITY_CODES = {
    0x01: "WARNING",
    0x02: "ERROR",
}

def calc_checksum(data: bytes) -> int:
    """XOR checksum matching sender C code."""
    crc = 0
    for b in data[1:-1]:  #skip header, include everything except CRC
        crc ^= b
    return crc & 0xFF


def parse_packet(buf: bytes):
    if len(buf) < 9 or buf[0] != PACKET_HEADER:
        return None

    seq = (buf[1] << 8) | buf[2]
    length = buf[3]
    if len(buf) != length:
        print(f"[WARN] Length mismatch: expected {length}, got {len(buf)}")
        return None

    crc_calc = calc_checksum(buf)
    crc_recv = buf[-1]
    if crc_calc != crc_recv:
        print(f"[WARN] CRC mismatch: expected {crc_calc:02X}, got {crc_recv:02X}")
        print(f"[DEBUG] Raw packet: {buf.hex(' ')}")
        return None

    timestamp = struct.unpack_from(">I", buf, 4)[0]
    pkt_type = buf[8]

    if pkt_type == PACKET_TYPE_TELEMETRY:
        offset = 9
        acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z = struct.unpack_from(">hhhhhh", buf, offset)
        offset += 12
        altitude = struct.unpack_from(">I", buf, offset)[0]
        offset += 4
        event_flags = buf[offset]
        sys_state = buf[offset + 1]

        return ("telemetry", {
            "seq": seq,
            "timestamp": timestamp,
            "acc_x": acc_x, "acc_y": acc_y, "acc_z": acc_z,
            "gyro_x": gyro_x, "gyro_y": gyro_y, "gyro_z": gyro_z,
            "altitude": altitude,
            "event_flags": event_flags,
            "sys_state": sys_state
        })

    elif pkt_type == PACKET_TYPE_ERROR:
        severity = buf[9]
        err_code = buf[10]

        return ("error", {
            "seq": seq,
            "timestamp": timestamp,
            "severity": severity,
            "severity_str": SEVERITY_CODES.get(severity, f"UNKNOWN({severity})"),
            "err_code": err_code,
            "err_code_str": ERROR_CODES.get(err_code, f"UNKNOWN({err_code})")
        })

    elif pkt_type == PACKET_TYPE_INFO:
        info_code = buf[9]
        return ("info", {
            "seq": seq,
            "timestamp": timestamp,
            "info_code": info_code,
            "info_code_str": INFO_CODES.get(info_code, f"UNKNOWN({info_code})")
        })

    elif pkt_type == PACKET_TYPE_DEBUG:
        severity = buf[9]
        err_code = buf[10]

        # Calculate optional message length
        msg_len = length - (11 + 1)  # 11 bytes header (up to code) + CRC
        if msg_len > 0:
            message_bytes = buf[11:11+msg_len]
            message = message_bytes.decode(errors="replace")
        else:
            message = None

        return ("debug", {
            "seq": seq,
            "timestamp": timestamp,
            "severity": severity,
            "severity_str": SEVERITY_CODES.get(severity, f"UNKNOWN({severity})"),
            "err_code": err_code,
            "err_code_str": ERROR_CODES.get(err_code, f"UNKNOWN({err_code})"),
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
    ser = serial.Serial(PORT, BAUD, timeout=0.1)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    time.sleep(2)

    print("\nConfiguring module with AT commands...")
    send_at_command(ser, "AT")
    send_at_command(ser, "AT+MODE=TEST")
    send_at_command(ser, "AT+TEST=RXLRPKT")

    print("\nModule configured. Listening for packets...\n")

    regex_rx = re.compile(r'\+TEST:\s*RX\s*"([0-9A-Fa-f]+)"')

    while True:
        line = ser.readline().decode(errors="ignore").strip()
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
                        print(f"[TLM] seq={d['seq']} ts={d['timestamp']} alt={d['altitude']}")
                        print(f"      acc=({d['acc_x']},{d['acc_y']},{d['acc_z']})")
                        print(f"      gyro=({d['gyro_x']},{d['gyro_y']},{d['gyro_z']})")
                        print(f"      event_flags={d['event_flags']:02X} sys_state={d['sys_state']:02X}")
                    elif pkt_type == "error":
                        print(f"[ERR] seq={d['seq']} ts={d['timestamp']} severity={d['severity_str']} code={d['err_code_str']}")
                    elif pkt_type == "info":
                        print(f"[INFO] seq={d['seq']} ts={d['timestamp']} code={d['info_code_str']}")
                    elif pkt_type == "debug":
                        print(f"[DEBUG] seq={d['seq']} ts={d['timestamp']} severity={d['severity_str']} code={d['err_code_str']}")
                        if d['message']:
                            print(f"        msg: {d['message']}")
            except Exception as e:
                print(f"[ERROR] Failed to parse RX hex: {e}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nStopped.")

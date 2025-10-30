import zmq
import time
import json
import pandas as pd
import threading
import sys, select, tty, termios
import math

def get_char():
    """Non-blocking single-character input"""
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)
    try:
        tty.setraw(fd)
        [i, _, _] = select.select([sys.stdin], [], [], 0.1)
        if i:
            return sys.stdin.read(1)
        return None
    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

# Required columns for BME280 + BNO055
REQUIRED_COLUMNS = [
    "temperature", "pressure", "humidity",
    "accel_x", "accel_y", "accel_z",
    "gyro_x", "gyro_y", "gyro_z",
    "heading", "roll", "pitch",
    "timestamp"
]

# Command-line arguments
if len(sys.argv) < 2:
    print("Usage: python csv_publisher.py <sensor_data.csv> [--loop]")
    sys.exit(1)

csv_file = sys.argv[1]
loop_flag = "--loop" in sys.argv

# Load CSV data
try:
    df = pd.read_csv(csv_file)
except Exception as e:
    print(f"Error reading CSV file: {e}")
    sys.exit(1)

# Validate columns
missing_cols = [c for c in REQUIRED_COLUMNS if c not in df.columns]
if missing_cols:
    print(f"Error: CSV file is missing required columns: {missing_cols}")
    sys.exit(1)

context = zmq.Context()
sock = context.socket(zmq.PUB)
sock.bind("tcp://*:5555")

# Print instructions
print("="*60)
print(f"CSV Sensor Publisher Ready! Using file: {csv_file}")
print(f"Looping: {'ON' if loop_flag else 'OFF'}")
print("ZMQ PUB running on tcp://*:5555")
print("\nControls:")
print("  r : Start / Resume playback")
print("  p : Pause playback")
print("  + : Double playback speed")
print("  - : Halve playback speed")
print("  f : Jump forward 10 rows")
print("  b : Jump backward 10 rows")
print("  q : Quit publisher")
print("="*60)
print("Publisher starts in PAUSED state. Press 'r' to begin.\n")

# Playback control variables
playback_speed = 1.0
paused = True
jump_rows = 10
current_index = [0]  # thread-safe index
running = True       # global flag for quitting

def input_thread():
    """Handles keyboard input for playback control"""
    global playback_speed, paused, running
    while running:
        ch = get_char()
        if ch:
            if ch == 'q':  # Quit
                print("Exiting publisher...")
                running = False
                break
            elif ch == 'p':  # Pause/resume
                paused = not paused
                print("Paused" if paused else "Resumed")
            elif ch == 'r':  # Start/resume playback
                paused = False
                print("Playback started/resumed")
            elif ch == '+':  # Speed up
                playback_speed *= 2
                print(f"Playback speed: {playback_speed}x")
            elif ch == '-':  # Slow down
                playback_speed /= 2
                print(f"Playback speed: {playback_speed}x")
            elif ch == 'f':  # Jump forward
                current_index[0] = min(current_index[0] + jump_rows, len(df)-1)
                print(f"Jumped forward to row {current_index[0]}")
            elif ch == 'b':  # Jump backward
                current_index[0] = max(current_index[0] - jump_rows, 0)
                print(f"Jumped backward to row {current_index[0]}")

# Start input listener
threading.Thread(target=input_thread, daemon=True).start()

# Main streaming loop
while running:
    if current_index[0] >= len(df):
        if loop_flag:
            current_index[0] = 0
            print("Restarting CSV playback (looping)...")
        else:
            print("End of CSV reached. Stopping publisher.")
            break  # stop after one pass

    i = current_index[0]
    row = df.loc[i]

    # Build sensor messages
    bme280 = {
        "temperature": row["temperature"],
        "pressure": row["pressure"],
        "humidity": row["humidity"],
        "timestamp": row["timestamp"]
    }

    bno055 = {
        "accel": [row["accel_x"], row["accel_y"], row["accel_z"]],
        "gyro": [row["gyro_x"], row["gyro_y"], row["gyro_z"]],
        "euler": {"heading": row["heading"], "roll": row["roll"], "pitch": row["pitch"]},
        "timestamp": row["timestamp"]
    }

    # Wait if paused
    while paused and running:
        time.sleep(0.1)

    if not running:
        break  # exit cleanly

    # Send ZMQ
    sock.send_multipart([b"BME280", json.dumps(bme280).encode()])
    sock.send_multipart([b"BNO055", json.dumps(bno055).encode()])

    # Compute acceleration magnitude
    ax, ay, az = bno055["accel"]
    accel_mag = math.sqrt(ax**2 + ay**2 + az**2)

    # Wait according to timestamp difference
    if i < len(df) - 1:
        next_row = df.loc[i + 1]
        wait_time = max(0, next_row["timestamp"] - row["timestamp"]) / playback_speed
        time.sleep(wait_time)

    # Move to next row
    current_index[0] += 1

print("Publisher exited successfully.")

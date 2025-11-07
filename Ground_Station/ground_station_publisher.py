import zmq
import time
import json
import random

context = zmq.Context()
sock = context.socket(zmq.PUB)
sock.bind("tcp://*:5555")

print("JSON sensor publisher started on tcp://*:5555")

count = 0
while True:
    now = time.time()

    telemetry = {
        "seq": count,
        "timestamp": now,
        "acc_x": random.uniform(-1, 1),
        "acc_y": random.uniform(-1, 1),
        "acc_z": random.uniform(-1, 1),
        "gyro_x": random.uniform(-1, 1),
        "gyro_y": random.uniform(-1, 1),
        "gyro_z": random.uniform(-1, 1),
        "pressure": 1000 + random.uniform(-5, 5),
        "altitude": 10 + random.uniform(-1, 1),
        "fsm_state": random.randint(0, 5),
        "fsm_state_str": "STATE_" + str(random.randint(0, 5))
    }

    # random throw error message every 5 counts
    if count % 5 == 0:
        log_json = {
            "seq": count,
            "timestamp": now,
            "level": "ERROR",
            "message": f"Test error message number {count}"
        }
    else:
        log_json = {
            "seq": count,
            "timestamp": now,
            "level": "INFO",
            "message": f"Test info message number {count}"
    }

    count += 1

    sock.send_multipart([b"TELEMETRY", json.dumps(telemetry).encode()])
    sock.send_multipart([b"LOG", json.dumps(log_json).encode()])

    time.sleep(1)

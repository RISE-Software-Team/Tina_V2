import zmq
import time
import json
import random

context = zmq.Context()
sock = context.socket(zmq.PUB)
sock.bind("tcp://*:5555")

print("JSON sensor publisher started on tcp://*:5555")

while True:
    now = time.time()

    bme280 = {
        "temperature": 25 + random.uniform(-1, 1),
        "pressure": 1000 + random.uniform(-5, 5),
        "humidity": 50 + random.uniform(-3, 3),
        "timestamp": now
    }

    bno055 = {
        "accel": [random.uniform(-1, 1) for _ in range(3)],
        "gyro": [random.uniform(-180, 180) for _ in range(3)],
        "mag": [random.uniform(-50, 50) for _ in range(3)],
        "timestamp": now
    }

    sock.send_multipart([b"BME280", json.dumps(bme280).encode()])
    sock.send_multipart([b"BNO055", json.dumps(bno055).encode()])

    time.sleep(1)

import json
import time
import logging
import datetime
import zmq
from math import sin, cos

import foxglove
from foxglove import Channel, Schema
from foxglove.schemas import (
    FrameTransforms,
    FrameTransform,
    Timestamp,
    Quaternion,
)

# ----------------- ZMQ SUBSCRIBER SETUP -----------------
context = zmq.Context()
sock = context.socket(zmq.SUB)
sock.connect("tcp://localhost:5555")
sock.setsockopt_string(zmq.SUBSCRIBE, "BME280")
sock.setsockopt_string(zmq.SUBSCRIBE, "BNO055")
print("✅ ZMQ Subscriber connected to tcp://localhost:5555")
print("Subscribed to: BME280, BNO055")


# ----------------- FOXGLOVE SERVER SETUP -----------------
foxglove.set_log_level(logging.INFO)
server = foxglove.start_server(
    capabilities=[],
    supported_encodings=["json"],
)
print("✅ Foxglove server started at ws://localhost:8765")

# Schema for BME280 messages
bme_schema = {
    "type": "object",
    "properties": {
        "timestamp": {"type": "number"},
        "temperature": {"type": "number"},
        "pressure": {"type": "number"},
        "humidity": {"type": "number"},
    },
}

# Schema for BNO055 messages
bno_schema = {
    "type": "object",
    "properties": {
        "timestamp": {"type": "number"},
        "accel": {"type": "array", "items": {"type": "number"}},
        "gyro": {"type": "array", "items": {"type": "number"}},
        "euler": {
            "type": "object",
            "properties": {
                "heading": {"type": "number"},
                "roll": {"type": "number"},
                "pitch": {"type": "number"},
            },
        },
    },
}

# Define channels
bme_chan = Channel(
    topic="/bme280",
    message_encoding="json",
    schema=Schema(name="bme280", encoding="jsonschema", data=json.dumps(bme_schema).encode("utf-8")),
)
bno_chan = Channel(
    topic="/bno055",
    message_encoding="json",
    schema=Schema(name="bno055", encoding="jsonschema", data=json.dumps(bno_schema).encode("utf-8")),
)


# ----------------- HELPER FUNCTION -----------------
def euler_to_quaternion(roll, pitch, yaw) -> Quaternion:
    roll, pitch, yaw = roll * 0.5, pitch * 0.5, yaw * 0.5
    sin_r, cos_r = sin(roll), cos(roll)
    sin_p, cos_p = sin(pitch), cos(pitch)
    sin_y, cos_y = sin(yaw), cos(yaw)
    w = cos_r * cos_p * cos_y + sin_r * sin_p * sin_y
    x = sin_r * cos_p * cos_y - cos_r * sin_p * sin_y
    y = cos_r * sin_p * cos_y + sin_r * cos_p * sin_y
    z = cos_r * cos_p * sin_y - sin_r * sin_p * cos_y
    return Quaternion(x=x, y=y, z=z, w=w)


# ----------------- MAIN LOOP -----------------
try:
    while True:
        topic, message = sock.recv_multipart()
        topic = topic.decode()
        data = json.loads(message.decode())

        # Forward data to Foxglove
        if topic == "BME280":
            bme_chan.log(data)

        elif topic == "BNO055":
            bno_chan.log(data)

            # Also visualize IMU orientation in 3D scene
            euler = data.get("euler", {})
            quat = euler_to_quaternion(
                euler.get("roll", 0),
                euler.get("pitch", 0),
                euler.get("heading", 0),
            )

            foxglove.log(
                "/tf",
                FrameTransforms(
                    transforms=[
                        FrameTransform(
                            parent_frame_id="world",
                            child_frame_id="imu",
                            rotation=quat,
                            timestamp=Timestamp.from_datetime(datetime.datetime.utcnow()),
                        )
                    ]
                ),
            )

except KeyboardInterrupt:
    print("\n🛑 Exiting...")
    server.stop()
    sock.close()

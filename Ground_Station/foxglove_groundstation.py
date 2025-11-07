import json
import time
import logging
import datetime
import zmq
from math import sin, cos

import foxglove
from foxglove import Channel, Schema
from foxglove.schemas import (
    Timestamp,
    LogLevel
)

# ----------------- ZMQ SUBSCRIBER SETUP -----------------
context = zmq.Context()
sock = context.socket(zmq.SUB)
sock.connect("tcp://localhost:5555")
sock.setsockopt_string(zmq.SUBSCRIBE, "TELEMETRY")
sock.setsockopt_string(zmq.SUBSCRIBE, "LOG")
print("✅ ZMQ Subscriber connected to tcp://localhost:5555")
print("Subscribed to: TELEMETRY, LOG")


# ----------------- FOXGLOVE SERVER SETUP -----------------
foxglove.set_log_level(logging.INFO)
server = foxglove.start_server(
    capabilities=[],
    supported_encodings=["json"],
)
print("✅ Foxglove server started at ws://localhost:8765")
    
telemetry_schema = {
    "type": "object",
    "properties": {
        "seq": {"type": "number"},
        "timestamp": {"type": "number"},
        "acc_x": {"type": "number"},
        "acc_y": {"type": "number"},
        "acc_z": {"type": "number"},
        "gyro_x": {"type": "number"},
        "gyro_y": {"type": "number"},
        "gyro_z": {"type": "number"},
        "pressure": {"type": "number"},
        "altitude": {"type": "number"},
        "fsm_state": {"type": "number"},
        "fsm_state_str": {"type": "string"},
    },
}

log_schema = {
    "type": "object",
    "properties": {
        "seq": {"type": "number"},
        "timestamp": {"type": "number"},
        "level": {"type": "LogLevel"},
        "message": {"type": "string"},
    },
}


telemetry_chan = Channel(
    topic="/telemetry",
    message_encoding="json",
    schema=Schema(name="telemetry", encoding="jsonschema", data=json.dumps(telemetry_schema).encode("utf-8")),
)

log_chan = Channel(
    topic="/log",
    message_encoding="json",
    schema=Schema(name="log", encoding="jsonschema", data=json.dumps(log_schema).encode("utf-8")),
)


try:
    while True:
        topic, message = sock.recv_multipart()
        topic = topic.decode()
        data = json.loads(message.decode())

        # Forward data to Foxglove
        if topic == "TELEMETRY":
            foxglove.log("/telemetry", data)
        elif topic == "LOG":
            foxglove.log("/log", data)

except KeyboardInterrupt:
    print("\n🛑 Exiting...")
    server.stop()
    sock.close()

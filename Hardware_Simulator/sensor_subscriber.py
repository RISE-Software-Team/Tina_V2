import zmq
import json

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:5555")

# Subscribe to BME280 messages
socket.setsockopt_string(zmq.SUBSCRIBE, "BME280")

print("Subscriber connected to tcp://localhost:5555")

while True:
    try:
        topic, message = socket.recv_multipart()
        # message is bytes, decode and parse JSON
        data = json.loads(message.decode())
        temp = data.get("temperature")
        press = data.get("pressure")
        hum = data.get("humidity")
        print(f"[{topic.decode()}] Timestamp: {data.get('timestamp')} | Temp: {temp:.2f} °C | Pressure: {press:.2f} hPa | Humidity: {hum:.2f} %")
    except KeyboardInterrupt:
        print("Exiting subscriber...")
        break
    except Exception as e:
        print("Error:", e)

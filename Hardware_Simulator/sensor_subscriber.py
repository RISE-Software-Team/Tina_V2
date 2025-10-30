import zmq
import json

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:5555")

# Subscribe to both BME280 and BNO055 messages
socket.setsockopt_string(zmq.SUBSCRIBE, "BME280")
socket.setsockopt_string(zmq.SUBSCRIBE, "BNO055")

print("Subscriber connected to tcp://localhost:5555")
print("Subscribed to: BME280, BNO055")
print("-" * 80)

while True:
    try:
        topic, message = socket.recv_multipart()
        topic_str = topic.decode()
        data = json.loads(message.decode())
        
        if topic_str == "BME280":
            # Parse BME280 data
            temp = data.get("temperature")
            press = data.get("pressure")
            hum = data.get("humidity")
            timestamp = data.get("timestamp")
            print(f"[BME280] Time: {timestamp:.2f} | Temp: {temp:.2f}°C | Press: {press:.2f}hPa | Hum: {hum:.2f}%")
        
        elif topic_str == "BNO055":
            # Parse BNO055 data
            accel = data.get("accel", [0, 0, 0])
            gyro = data.get("gyro", [0, 0, 0])
            euler = data.get("euler", {})
            timestamp = data.get("timestamp")
            
            print(f"[BNO055] Time: {timestamp:.2f}")
            print(f"  Accel: X={accel[0]:6.2f} Y={accel[1]:6.2f} Z={accel[2]:6.2f} m/s²")
            print(f"  Gyro:  X={gyro[0]:6.2f} Y={gyro[1]:6.2f} Z={gyro[2]:6.2f} °/s")
            print(f"  Euler: H={euler.get('heading', 0):6.2f}° R={euler.get('roll', 0):6.2f}° P={euler.get('pitch', 0):6.2f}°")
            print()
        
    except KeyboardInterrupt:
        print("\nExiting subscriber...")
        break
    except Exception as e:
        print(f"Error: {e}")

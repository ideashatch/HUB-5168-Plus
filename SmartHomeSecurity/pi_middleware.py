# 該程式為中介軟體，負責接收來自感測器的資料，並將資料傳送至API
# 主要在樹梅派上執行，並透過MQTT協定與感測器溝通
# 注意 樹梅派本身(LOCALHOST)須設置MOSQUITTO BROKER
# pip install paho-mqtt
import json
import requests
import paho.mqtt.client as mqtt
import time

API_URL = "https://以遮蔽/y/work"
API_WARNING_URL = "https://以遮蔽/y/warning"
MQTT_BROKER_HOST = "localhost"
MQTT_BROKER_PORT = 1883
MQTT_TOPIC_LIGHT1 = "home/light1"
MQTT_TOPIC_LIGHT2 = "home/light2"
MQTT_TOPIC_SENSOR1 = "home/sensor1"  # temperature
MQTT_TOPIC_SENSOR2 = "home/sensor2"  # humidity
MQTT_TOPIC_SENSOR3 = "home/sensor3"  # gas sensor
MQTT_TOPIC_SENSOR4 = "home/sensor4"  # PIR Motion Sensor
# test=True
light_state = {"light1": False, "light2": False}


def on_message(client, userdata, message):
    topic = message.topic
    payload = message.payload.decode("utf-8")
    print("Received message:", topic, payload)
    # if topic == MQTT_TOPIC_LIGHT1 or topic == MQTT_TOPIC_LIGHT2:
    #     update_light_state(topic, payload)
    # el
    if topic in [MQTT_TOPIC_SENSOR1, MQTT_TOPIC_SENSOR2]:
        send_sensor_data(topic, payload)
    elif topic in [MQTT_TOPIC_SENSOR3, MQTT_TOPIC_SENSOR4]:
        send_warning_data(topic, payload)


def update_light_state(client):
    response = requests.get(API_URL)
    current_state = response.json()
    for light, state in current_state.items():
        if state != light_state[light]:
            light_state[light] = state
            topic = MQTT_TOPIC_LIGHT1 if light == "light1" else MQTT_TOPIC_LIGHT2
            client.publish(topic, state)
            print("Light state updated:", light, state)


def send_sensor_data(topic, sensor_data):  # temperature humidity
    payload = {}
    if MQTT_TOPIC_SENSOR1 == topic:
        payload["temperature"] = sensor_data
    elif MQTT_TOPIC_SENSOR2 == topic:
        payload["humidity"] = sensor_data
    response = requests.get(API_URL, params=payload)
    print("Sensor data sent:", response.text)


def send_warning_data(topic, sensor_data):  # gas sensor PIR Motion Sensor
    payload = {}
    if MQTT_TOPIC_SENSOR3 == topic:
        payload["gas_sensor"] = sensor_data
    elif MQTT_TOPIC_SENSOR4 == topic:
        if sensor_data == '':
            sensor_data = 0
        payload["PIR"] = sensor_data
        sensor_data = int(sensor_data)
        if sensor_data < 30:
            payload["template"] = 3
        elif sensor_data < 150:
            payload["template"] = 2
        else:
            payload["template"] = 1
    # send "template" to API to order sending warning message
    # 1: pir enable!
    # 2: pit enable! & distance < 150
    # 3: pit enable! & distance < 30

    response = requests.get(API_WARNING_URL, params=payload)
    print("Sensor data sent:", response.text)


def main():
    client = mqtt.Client()
    client.on_message = on_message

    client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT)
    client.subscribe([(MQTT_TOPIC_LIGHT1, 0), (MQTT_TOPIC_LIGHT2, 0),
                     (MQTT_TOPIC_SENSOR1, 0), (MQTT_TOPIC_SENSOR2, 0), (MQTT_TOPIC_SENSOR3, 0), (MQTT_TOPIC_SENSOR4, 0)])
    client.loop_start()
    # count=0
    # if test:
    #     for i in range(100):
    #         client.publish(MQTT_TOPIC_LIGHT1, "on"if i%2==0 else "off")
    #         client.publish(MQTT_TOPIC_LIGHT2, "off"if i%2==0 else "on")

    #         count+=1
    #         print(count)
    #         time.sleep(1)
    try:
        while True:
            # response = requests.get(API_URL)
            # current_state = response.json()

            # for light, state in current_state.items():
            #     if light == "light1":
            #         topic = MQTT_TOPIC_LIGHT1
            #     elif light == "light2":
            #         topic = MQTT_TOPIC_LIGHT2
            #     else:
            #         continue
            #     client.publish(topic, state)
            update_light_state(client)

            time.sleep(0.010)
    except KeyboardInterrupt:
        client.disconnect()
        client.loop_stop()


if __name__ == "__main__":
    main()

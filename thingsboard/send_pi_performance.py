import paho.mqtt.client as mqtt
import json
import time
import psutil

THINGSBOARD_HOST = '34.69.172.61'
MQTT_PORT = 1884
ACCESS_TOKEN = 'tZ7q90oxzKorXFghVm77'

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, *extra_params):
    print('Connected with result code ' + str(rc))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print ('Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))

def send_pi_performance():
    data = {}

    tempC = psutil.sensors_temperatures().get('cpu-thermal', [[0, 0]])[0][1]
    
    data['cpu_thermal'] = int(tempC)
    data['cpu_usage'] =  int(psutil.cpu_percent())

    memory = psutil.virtual_memory()
    # Divide from Bytes -> KB -> MB
    # available = round(memory.available/1024.0/1024.0,1)
    # total = round(memory.total/1024.0/1024.0,1)
    data['mem_usage'] = int(memory.percent)

    disk = psutil.disk_usage('/')
    # Divide from Bytes -> KB -> MB -> GB
    # free = round(disk.free/1024.0/1024.0/1024.0,1)
    # total = round(disk.total/1024.0/1024.0/1024.0,1)
    data['disk_usage'] = int(disk.percent)


    json_data = json.dumps(data)

    print("publish: ", json_data)
    client.publish('v1/devices/me/telemetry', json_data, 1)


client = mqtt.Client(protocol=mqtt.MQTTv311)
# Register connect callback
client.on_connect = on_connect
# Registed publish message callback
client.on_message = on_message
# Set access token
client.username_pw_set(ACCESS_TOKEN)
# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(THINGSBOARD_HOST, MQTT_PORT, 60)

try:
    client.loop_start()
    while(True):
        #if is_connected == True:
        send_pi_performance()
        time.sleep(1)

except KeyboardInterrupt:
    #GPIO.cleanup()
    pass


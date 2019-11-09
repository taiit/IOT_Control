import paho.mqtt.client as mqtt
import json
import time
import psutil
import pdb

THINGSBOARD_HOST = '34.69.172.61'
MQTT_PORT = 1884
ACCESS_TOKEN = 'tZ7q90oxzKorXFghVm77'

gpio_state = {7: False, 11: False, 12: False, 13: False, 15: False, 16: False, 18: False, 22: False, 29: False,
               31: False, 32: False, 33: False, 35: False, 36: False, 37: False, 38: False, 40: False}

def get_gpio_status():
    # Encode GPIOs state to json
    return json.dumps(gpio_state)


def set_gpio_status(pin, status):
    # Output GPIOs state
    #GPIO.output(pin, GPIO.HIGH if status else GPIO.LOW)
    # Update GPIOs state
    gpio_state[pin] = status

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, *extra_params):
    print('Connected with result code ' + str(rc))
    print('Subscribe meg v1/devices/me/rpc/request/+')
    
    client.subscribe('v1/devices/me/rpc/request/+')
    
    client.publish('v1/devices/me/attributes', get_gpio_status(), 1)

    data = {'getValue': True}
    client.publish('v1/devices/me/attributes', json.dumps(data))

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print ('Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))
    # Decode JSON request
    data = json.loads(msg.payload)
    # Check request method
    if data['method'] == 'getGpioStatus':
        # Reply with GPIO status
        client.publish(msg.topic.replace('request', 'response'), get_gpio_status(), 1)
    elif data['method'] == 'setGpioStatus':
        # Update GPIO status and reply
        set_gpio_status(data['params']['pin'], data['params']['enabled'])
        client.publish(msg.topic.replace('request', 'response'), get_gpio_status(), 1)
        client.publish('v1/devices/me/attributes', get_gpio_status(), 1)
    elif data['method'] == 'setValue':
        if data['params'] == True:
            print("ON")
        else:
            print("OFF")
    elif data['method'] == 'getValue':
        data = {'getValue': True}
        client.publish(msg.topic.replace('request', 'response'), json.dumps(data), 1)

def send_pi_performance():
    data = {}
  
    #pdb.set_trace()
    tempC = 0
    if hasattr(psutil, 'sensors_temperatures'):
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

    #print("publish: ", json_data)
    client.publish('v1/devices/me/telemetry', json_data, 1)


#client = mqtt.Client(protocol=mqtt.MQTTv311)
client = mqtt.Client()
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


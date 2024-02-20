import serial
import paho.mqtt.client as mqtt
import json
import time

# Conexión MQTT
broker_address = "iot.eie.ucr.ac.cr"
mqtt_port = 1883
mqtt_topic = "v1/devices/me/telemetry"
mqtt_password = "yq7cgc21emlixma2q1p0"

# Se configura el puerto serie
serial_port = "/dev/ttyACM0"  
baudrate = 115200

# Crear instancia del cliente MQTT
mqtt_client = mqtt.Client("Arduino_Alarm_System")

# Definir los manejadores de eventos MQTT
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("Connected to MQTT broker")
    else:
        print(f"Failed to connect to MQTT broker with result code {rc}")

def on_disconnect(client, userdata, rc):
    print(f"Disconnected from MQTT broker with result code {rc}")

# Funciones de callback
mqtt_client.on_connect = on_connect
mqtt_client.on_disconnect = on_disconnect

# Nombre de usuario y contraseña para MQTT
mqtt_client.username_pw_set(mqtt_password, "")

# Conectar al broker MQTT
mqtt_client.connect(broker_address, mqtt_port, 60)
mqtt_client.loop_start()

# Iniciar la comunicación serial
comunicacion_serial = serial.Serial(serial_port, baudrate, timeout=1)

while True:
    try:
        if comunicacion_serial.inWaiting():
            linea = comunicacion_serial.readline().decode('utf-8').strip()
            datos = json.loads(linea)  # Asumiendo que Arduino envía un JSON válido

            print(datos)  # Imprime los datos recibidos para depuración

            try:
                mqtt_client.publish(mqtt_topic, linea)  # Envía el JSON tal cual fue recibido
            except ValueError:
                print("Error al procesar los datos recibidos.")

    except KeyboardInterrupt:
        print("\nScript interrumpido por el usuario.")
        break

# Limpiar antes de salir
comunicacion_serial.close()
mqtt_client.loop_stop()
mqtt_client.disconnect()

#!/usr/bin/env python3

import serial
import paho.mqtt.client as mqtt
import json
import time

# Conexión MQTT
broker_address = "iot.eie.ucr.ac.cr"
mqtt_port = 1883
#mqtt_port = 8080
mqtt_topic = "v1/devices/me/telemetry"
#mqtt_username = "Lab4_C07893_B63561"
mqtt_password = "dzmt5slmkremxaqgwin5"


x = 0

# Se configura el puerto serie
#serial_port = "/dev/ttyACM0"  
#baudrate = 115200  # Velocidad de baudios
serial_port = "COM6"  
baudrate = 9600  # Velocidad de baudios

# Crear instancia del cliente MQTT
mqtt_client = mqtt.Client("STM32_Serial_MQTT")

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
mqtt_client.subscribe(mqtt_topic)
mqtt_client.loop_start()  # Iniciar el loop en un hilo separado

data = dict()

x=0
y=1
z=2
r=3

# Iniciar la comunicación serial
comunicacion_serial = serial.Serial(serial_port, baudrate, timeout=1)
data = dict()
while True:
    try:
        if comunicacion_serial.inWaiting():

            mensaje = comunicacion_serial.readline().decode('utf-8')
            #mensaje = comunicacion_serial.readline().decode().split()
            print(mensaje)  # Imprime el mensaje para depuración

            if mensaje == "Movimiento":
                data = {"Alarma1":"ON", "Alarma":"ON"}
            else:
                data = {"Alarma1":"OFF", "Alarma":"OFF"}

            #data_out = json.dumps(data)  # Create JSON object

            # Construir payload MQTT
                payload = json.dumps({"Eje X": x,
                                      "Eje Y": y,
                                      "Eje Z": z,
                                      "Nivel Bateria": r})

            try:
                # Construir payload MQTT
                mqtt_client.publish(mqtt_topic, payload)
               # mqtt_client.publish(mqtt_topic, data_out, qos=0)  # publish
            except ValueError:
                print("Error al procesar los datos recibidos.")

    except KeyboardInterrupt:
        print("\nScript interrumpido por el usuario.")
        break

# Limpiar antes de salir
comunicacion_serial.close()
mqtt_client.loop_stop()
mqtt_client.disconnect()
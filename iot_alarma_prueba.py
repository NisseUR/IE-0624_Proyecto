#!/usr/bin/env python3

import serial
import paho.mqtt.client as mqtt
import json
import time

# Configuración MQTT
broker_address = "iot.eie.ucr.ac.cr"
mqtt_port = 1883
mqtt_topic = "v1/devices/me/telemetry"
mqtt_password = "pb9655hkzckn38h3lmj3"

# Se configura el puerto serie
#serial_port = "/dev/ttyACM0" # Linux
serial_port = "COM6"  # Windows
#baudrate = 115200
baudrate = 9600

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

# Configurar cliente MQTT
mqtt_client.connect(broker_address, mqtt_port, 60)
mqtt_client.subscribe(mqtt_topic)
mqtt_client.loop_start()  # Iniciar el loop en un hilo separado

# Iniciar comunicación serial
comunicacion_serial = serial.Serial(serial_port, baudrate, timeout=1)

#estado = "desconocido"
#modo = "Ninguno"
#movimiento = "no detectado"
#print("HOLA")

try:
    while True:
        if comunicacion_serial.inWaiting():
            mensaje = comunicacion_serial.readline().decode('utf-8').strip()
            print(mensaje)  # Imprime el mensaje para depuración

            if mensaje == "Movimiento detectado en la zona.":
                data="ON"
                data_out=json.dumps(data) #create JSON object
                print("data out= ",data_out)
                mqtt_client.publish(mqtt_topic, data_out) #publish
            else:
                data="OFF"
                data_out=json.dumps(data) #create JSON object
                #print("data out= ",data_out)
                mqtt_client.publish(mqtt_topic, data_out) #publish

            #if mensaje == "Sistema activado":
             #   estado = "activado"
            #elif "Modo seleccionado:" in mensaje:
             #   modo = mensaje.split(": ")[1]  # Extrae el modo del mensaje
            #elif mensaje == "Movimiento detectado en la zona.":
             #   movimiento = "detectado"
             #   data_out=json.dumps(movimiento) #create JSON object
             #   mqtt_client.publish(mqtt_topic, data_out)
            #elif mensaje.startswith("\nModo desactivado"):
             #   estado = "desactivado"
             #   movimiento = "no detectado"  # Resetear el estado de movimiento al desactivar
            #try:
                # Prepara el payload para ThingsBoard
             #   payload = {
             #       "estado": estado,
             #       "modo": modo,
             #       "movimiento": movimiento
             #   }
                #mqtt_client.publish(mqtt_topic, json.dumps(payload))
                #mqtt_client.publish(mqtt_topic, json.dumps(5))
            #except ValueError:
            #    print("Error al procesar los datos recibidos.")

        #time.sleep(1)

except KeyboardInterrupt:
    print("Script interrumpido por el usuario")

finally:
    comunicacion_serial.close()
    mqtt_client.loop_stop()
    mqtt_client.disconnect()


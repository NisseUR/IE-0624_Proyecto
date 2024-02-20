#!/usr/bin/env python3

import serial
import paho.mqtt.client as mqtt
import json
import time

# Configuración MQTT
broker_address = "iot.eie.ucr.ac.cr"
mqtt_port = 1883
mqtt_topic = "v1/devices/me/telemetry"
mqtt_password = "yq7cgc21emlixma2q1p0"

<<<<<<< HEAD
# Configuración del puerto serie
serial_port = "/dev/ttyACM0"
=======
# Se configura el puerto serie
#serial_port = "/dev/ttyACM0"
serial_port = "COM6"  # Windows
>>>>>>> 03a9fbd35ad1dd24215ffe280199acb3c0afdaa3
baudrate = 115200

# Crear instancia del cliente MQTT
mqtt_client = mqtt.Client("Arduino_Alarm_System")

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker with result code " + str(rc))

# Configurar cliente MQTT
mqtt_client.on_connect = on_connect
mqtt_client.username_pw_set(mqtt_password, "")
mqtt_client.connect(broker_address, mqtt_port, 60)

# Iniciar el loop MQTT en un hilo separado
mqtt_client.loop_start()

# Iniciar comunicación serial
comunicacion_serial = serial.Serial(serial_port, baudrate, timeout=1)

estado = "desconocido"
modo = "Ninguno"
movimiento = "no detectado"

try:
    while True:
        if comunicacion_serial.inWaiting() > 0:
            mensaje = comunicacion_serial.readline().decode('utf-8').strip()
            print(mensaje)  # Imprime el mensaje para depuración

            if mensaje == "Sistema activado":
                estado = "activado"
            elif "Modo seleccionado:" in mensaje:
                modo = mensaje.split(": ")[1]  # Extrae el modo del mensaje
            elif mensaje == "Movimiento detectado en la zona.":
                movimiento = "detectado"
            elif mensaje.startswith("\nModo desactivado"):
                estado = "desactivado"
                movimiento = "no detectado"  # Resetear el estado de movimiento al desactivar
            
            # Prepara el payload para ThingsBoard
            payload = {
                "estado": estado,
                "modo": modo,
                "movimiento": movimiento
            }
            mqtt_client.publish(mqtt_topic, json.dumps(payload))

        time.sleep(1)

except KeyboardInterrupt:
    print("Script interrumpido por el usuario")

finally:
    comunicacion_serial.close()
    mqtt_client.loop_stop()
    mqtt_client.disconnect()


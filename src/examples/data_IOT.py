#!/usr/bin/env python3

import serial
import paho.mqtt.client as mqtt
import json
import time

# Conexión MQTT
broker_address = "iot.eie.ucr.ac.cr"
mqtt_port = 1883
mqtt_topic = "v1/devices/me/telemetry"
mqtt_password = "dzmt5slmkremxaqgwin5"


# Se configura el puerto serial
#serial_port = "/dev/ttyACM0"  
#baudrate = 115200
serial_port = "COM6"  
baudrate = 9600 

# Crear instancia del cliente MQTT
mqtt_client = mqtt.Client("STM32_Serial_MQTT")

def on_log(client, userdata, level, buf):
   print(buf) 

def on_publish(client, userdata, mid):
    print("In on_pub callback mid= "  ,mid)   

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

#mqtt_client.on_log=on_log

# Inicialización de los estados
estado = "Desactivado"
modo = "Ninguno"
movimiento = "NoDetectado"
alarma = "OFF"

# Iniciar la comunicación serial
comunicacion_serial = serial.Serial(serial_port, baudrate, timeout=1)

while True:
    try:
        if comunicacion_serial.inWaiting():

            mensaje = comunicacion_serial.readline().decode('utf-8')
            #mensaje = comunicacion_serial.readline().decode().split()
            print(mensaje)  # Imprime el mensaje para depuración

            if mensaje.strip() == "alarma":
                estado = "Activado"
                modo = "Alarma"
                #alarma = "OFF"
                #movimiento = "NoDetectado"
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            elif mensaje.strip() == "chime":
                estado = "Activado"
                modo = "Chime"
                #alarma = "OFF"
                #movimiento = "NoDetectado"
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            elif mensaje.strip() == "password":
                estado = "Desactivado"
                modo = "ChangingPassword"
                #alarma = "OFF"
                #movimiento = "NoDetectado"
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            elif mensaje.strip() == "movimiento":
                if modo == "Chime":
                    estado = "Activado"
                    alarma = "ON"
                    movimiento = "Detectado"
                else: # Alarma
                    estado = "Activado"
                    alarma = "ON"    
                    movimiento = "Detectado"
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            elif mensaje.strip() == "chimetermina":
                estado = "Activado"
                modo = "chime"
                alarma = "OFF"
                movimiento = "NoDetectado"
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            else:
                data = {"Estado": estado, "Modo": modo, "Movimiento": movimiento, "Alarma": alarma}
            
            # Construir payload MQTT usando los datos recibidos
            payload = json.dumps(data)

            try:
                # Publicar el mensaje MQTT
                mqtt_client.publish(mqtt_topic, payload, 0)
                #mqtt_client.loop()
            except ValueError:
                print("Error al procesar los datos recibidos.")

    except KeyboardInterrupt:
        print("\nScript interrumpido por el usuario.")
        break
    
# Limpiar antes de salir
comunicacion_serial.close()
mqtt_client.loop_stop()
mqtt_client.disconnect()
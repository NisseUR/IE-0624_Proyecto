# Proyecto Final: Sistema Alarma de detección de movimiento

## Denisse Ugalde Rivera C07893 & Alonso José Jiménez Anchía B63561

Connections for Arduino Mega

OV7675 connections:

VSYNC or VS - PIN49
XCLCK - PIN9 (must be level shifted from 5V -> 3.3V)
PCLCK - PIN47
SIOD - PIN20-SDA (I2C data) - 10K resistor to 3.3V
SIOC - PIN21-SCL (I2C clock) - 10K resistor to 3.3V
D0..D7 - PIN22..PIN29 (pixel data bits 0..7)
3.3V - 3.3V
RESET - 3.3V
GND - GND
PWDN - GND
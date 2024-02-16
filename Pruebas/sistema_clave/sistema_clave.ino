#include <Keypad.h>

// Definición de los pines y configuración del teclado
const byte ROWS = 4; // Cuatro filas
const byte COLS = 4; // Cuatro columnas
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; // Pines de las filas del teclado a Arduino
byte colPins[COLS] = {5, 4, 3, 2}; // Pines de las columnas del teclado a Arduino
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables para el sensor PIR y el buzzer
int ledPin = 13;
int pirPin = 10;
int buzzer = 12;
int pirState = LOW;
bool systemActive = false; // Estado inicial del sistema (desactivado)

String password = ""; // Contraseña ingresada
String correctPassword = "1234"; // Contraseña correcta

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sistema desactivado. Ingrese la contraseña para activar.");
}

void loop() {
  char key = customKeypad.getKey();
  
  // Procesamiento de la entrada del teclado
  if (key) {
    password += key;
    if (password.length() == 4) {
      if (password == correctPassword) {
        systemActive = !systemActive; // Cambia el estado de activación del sistema
        Serial.println(systemActive ? "Sistema activado." : "Sistema desactivado.");
        password = ""; // Restablece la contraseña para una nueva entrada
      } else {
        Serial.println("Contraseña incorrecta. Intente de nuevo.");
        password = ""; // Restablece la contraseña para una nueva entrada
      }
    }
  }

  // Lógica del sensor PIR
  pirState = digitalRead(pirPin);
  digitalWrite(ledPin, pirState); // El LED siempre refleja el estado del sensor PIR
  
  if (pirState == HIGH && systemActive) {
    digitalWrite(buzzer, HIGH); // Activa el buzzer solo si el sistema está activado
    Serial.println("Movimiento detectado!");
  } else {
    digitalWrite(buzzer, LOW); // Asegura que el buzzer esté apagado si no hay movimiento o el sistema está desactivado
  }
  
  delay(100); // Pequeño retardo para evitar múltiples lecturas
}

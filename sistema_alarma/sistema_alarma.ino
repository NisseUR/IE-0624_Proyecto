#include <Keypad.h>

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

int ledPin = 13;
int pirPin = 10;
int buzzer = 12;
bool systemActive = false; // Estado inicial del sistema (desactivado)
bool alarmTriggered = false; // Indica si la alarma fue activada por movimiento

String password = ""; // Contraseña ingresada
String correctPassword = "1234"; // Contraseña correcta

unsigned long previousMillis = 0; // Almacenará la última vez que el LED fue actualizado
const long interval = 500; // Intervalo en milisegundos para el parpadeo del LED

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sistema desactivado. Ingrese la contraseña para activar.");
}

void loop() {
  char key = customKeypad.getKey();
  if (key) {
    Serial.print(key); // Imprime cada dígito conforme se presiona
    password += key;
    
    if (password.length() == 4) {
      Serial.println(); // Salto de línea tras ingresar 4 dígitos
      
      if (password == correctPassword) {
        systemActive = !systemActive; // Cambia el estado de activación del sistema
        alarmTriggered = false; // Restablece el estado de la alarma
        digitalWrite(buzzer, LOW); // Apaga el buzzer
        Serial.println(systemActive ? "Sistema activado." : "Sistema desactivado. Alarma detenida.");
        password = ""; // Restablece la contraseña para una nueva entrada
      } else {
        Serial.println("Contraseña incorrecta. Intente de nuevo.");
        password = ""; // Restablece la contraseña para seguir intentando
      }
    }
  }

  // Control de parpadeo del LED solo cuando la alarma está activada y sonando
  if (alarmTriggered) {
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= interval) {
      // Si ha pasado el intervalo, cambia el estado del LED
      previousMillis = currentMillis;
      
      // Si el LED está encendido, apágalo y viceversa
      digitalWrite(ledPin, !digitalRead(ledPin));
    }
    
    digitalWrite(buzzer, HIGH); // Buzzer suena continuamente
  } else {
    // Detección de movimiento independiente del estado de la alarma
    int pirState = digitalRead(pirPin);
    if (systemActive && pirState == HIGH && !alarmTriggered) {
      alarmTriggered = true; // Activa la alarma
      Serial.println("¡Alarma! Movimiento detectado.");
    } else if (!systemActive && pirState == HIGH) {
      digitalWrite(ledPin, HIGH); // Enciende el LED si detecta movimiento (sistema desarmado)
      delay(200); // Pequeño retardo para visibilidad
      digitalWrite(ledPin, LOW);
    }
  }
}
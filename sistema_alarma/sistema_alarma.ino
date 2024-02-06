#include <Keypad.h>

const byte ROWS = 4; // Cuatro filas
const byte COLS = 4; // Cuatro columnas
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int ledPin = 13;
int pirPin = 7;
int buzzer = 12;
bool systemArmed = false;
bool alarmTriggered = false;

String inputPassword = "";
String correctPassword = "1234";

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Sistema desactivado. Ingrese la contraseña para activar.");
}

void loop() {
  char key = customKeypad.getKey();
  
  if (key != NO_KEY) {
    Serial.print(key); // Muestra cada tecla presionada
    inputPassword += key;

    if (inputPassword.length() == 4) {
      if (inputPassword == correctPassword) {
        if (systemArmed) {
          systemArmed = false;
          alarmTriggered = false;
          digitalWrite(buzzer, LOW); // Asegura apagar el buzzer
          Serial.println("\nSistema desarmado.");
        } else {
          systemArmed = true;
          Serial.println("\nSistema armado. Esperando movimiento...");
        }
        inputPassword = ""; // Resetea la contraseña ingresada
      } else {
        Serial.println("\nContraseña incorrecta. Intente de nuevo.");
        inputPassword = ""; // Resetea si la contraseña es incorrecta
      }
    }
  }
  
  // Solo verifica el sensor PIR si la alarma no ha sido activada
  if (!alarmTriggered) {
    if (digitalRead(pirPin) == HIGH) {
      if (systemArmed) {
        alarmTriggered = true;
        Serial.println("¡Alarma! Movimiento detectado.");
      } else {
        digitalWrite(ledPin, HIGH); // Enciende el LED si detecta movimiento (sistema desarmado)
        delay(200); // Pequeño retardo para visibilidad
        digitalWrite(ledPin, LOW);
      }
    }
  }
  
  // Manejo de la alarma activada
  if (alarmTriggered) {
    digitalWrite(ledPin, !digitalRead(ledPin)); // Parpadeo del LED
    digitalWrite(buzzer, HIGH); // Buzzer suena continuamente
    delay(100); // Controla la velocidad de parpadeo
  }
}

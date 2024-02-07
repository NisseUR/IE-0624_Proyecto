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
  
  if (key) {
    password += key; // Acumula los dígitos presionados
    Serial.print(key); // Imprime cada dígito conforme se presiona
    
    if (password.length() == 4) {
      // Después del último dígito, imprime un salto de línea
      Serial.println();
      
      if (password == correctPassword) {
        systemActive = !systemActive; // Cambia el estado de activación del sistema
        Serial.println(systemActive ? "Sistema activado." : "Sistema desactivado.");
      } else {
        Serial.println("Contraseña incorrecta. Intente de nuevo.");
      }
      password = ""; // Restablece la contraseña para una nueva entrada
    }
  }

  pirState = digitalRead(pirPin);
  digitalWrite(ledPin, pirState); // El LED refleja el estado del sensor PIR
  
  if (pirState == HIGH && systemActive) {
    digitalWrite(buzzer, HIGH); // Activa el buzzer si el sistema está activado y detecta movimiento
    Serial.println("Movimiento detectado!");
  } else {
    digitalWrite(buzzer, LOW); // Asegura que el buzzer esté apagado si no hay movimiento o el sistema está desactivado
  }
  
  delay(100); // Pequeño retardo para evitar múltiples lecturas
}

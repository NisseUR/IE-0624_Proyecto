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
const long buzzerInterval = 2000; // Duración del sonido para el modo Chime

enum OperationMode { NONE, CHIME, ALARM } mode = NONE;
bool modeSelected = false;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bienvenido al sistema de seguridad.");
  Serial.println("Seleccione el modo: A para Chime, B para Alarma.");
}

void loop() {
  if (!modeSelected) {
    selectMode();
  } else if (!systemActive) {
    enterPassword();
  } else {
    handleSensorAndAlerts();
    // Verificar la contraseña para desactivar si el sistema está activo
    if (systemActive) {
      checkPasswordToDeactivate();
    }
  }
}

void selectMode() {
  char key = customKeypad.getKey();
  if (key == 'A' || key == 'B') {
    mode = (key == 'A') ? CHIME : ALARM;
    Serial.print("Modo seleccionado: ");
    Serial.println(mode == CHIME ? "Chime" : "Alarma");
    modeSelected = true;
    Serial.println("Ingrese la contraseña para activar el sistema.");
  }
}

void enterPassword() {
  char key = customKeypad.getKey();
  if (key) {
    Serial.print(key); // Imprime cada dígito conforme se presiona
    password += key;
    
    if (password.length() == 4) {
      Serial.println(); // Salto de línea tras ingresar 4 dígitos
      
      if (password == correctPassword) {
        systemActive = true; // Activa el sistema
        Serial.println("Sistema activado.");
        password = ""; // Restablece la contraseña para una nueva entrada
      } else {
        Serial.println("Contraseña incorrecta. Intente de nuevo.");
        password = ""; // Restablece la contraseña para seguir intentando
      }
    }
  }
}

void handleSensorAndAlerts() {
  int pirState = digitalRead(pirPin);
  static unsigned long lastTriggerMillis = 0; // Guarda el momento del último disparo de alarma
  
  if (pirState == HIGH) {
    Serial.println("Movimiento detectado en la zona.");
    if (mode == CHIME && !alarmTriggered) {
      // En Modo Chime, activa el buzzer por 2 segundos al detectar movimiento
      alarmTriggered = true;
      digitalWrite(buzzer, HIGH);
      lastTriggerMillis = millis();
    } else if (mode == ALARM && !alarmTriggered) {
      // En Modo Alarma, inicia la alarma
      alarmTriggered = true;
      lastTriggerMillis = millis();
    }
  }
  
  // Para el Modo Chime, apaga el buzzer después de 2 segundos
  if (mode == CHIME && alarmTriggered && millis() - lastTriggerMillis >= 2000) {
    digitalWrite(buzzer, LOW);
    alarmTriggered = false; // Restablece para permitir nuevas detecciones
  }

  // Para el Modo Alarma, maneja el parpadeo del LED y el buzzer continuo
  if (mode == ALARM && alarmTriggered) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin)); // Parpadeo del LED
    }
    digitalWrite(buzzer, HIGH); // Mantiene el buzzer sonando
  }
}

bool checkPasswordToDeactivate() {
  char key = customKeypad.getKey();
  if (key) {
    password += key;
    if (password.length() == 4) {
      if (password == correctPassword) {
        deactivateSystem();
        return true;
      } else {
        Serial.println("Contraseña incorrecta. Intente de nuevo.");
        password = ""; // Restablece para seguir intentando
      }
    }
  }
  return false;
}

void deactivateSystem() {
  systemActive = false;
  modeSelected = false;
  alarmTriggered = false;
  digitalWrite(buzzer, LOW); // Apaga el buzzer
  digitalWrite(ledPin, LOW); // Apaga el LED
  Serial.println("Modo desactivado. Volviendo al menú principal.");
  Serial.println("Seleccione el modo: A para Chime, B para Alarma.");
  password = ""; // Restablece la contraseña
}
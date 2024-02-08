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
bool systemActive = false;
bool alarmTriggered = false;
String userPassword = ""; // Nueva variable para almacenar la contraseña del usuario
String tempPassword = ""; // Almacena temporalmente la contraseña mientras se ingresa
unsigned long previousMillis = 0;
const long interval = 500;

enum OperationMode { NONE, CHIME, ALARM } mode = NONE;
bool modeSelected = false;
bool passwordSet = false; // Indica si la contraseña del usuario ha sido establecida

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  Serial.println("Bienvenido al sistema de seguridad.");
  Serial.println("Por favor, ingrese una nueva clave de 4 dígitos/caracteres para el sistema:");
}

void loop() {
  if (!passwordSet) {
    setPassword();
  } else if (!modeSelected) {
    selectMode();
  } else if (!systemActive) {
    enterPassword();
  } else {
    handleSensorAndAlerts();
    // Llama a checkPasswordToDeactivate() independientemente del estado de alarmTriggered
    checkPasswordToDeactivate();
  }
}

void setPassword() {
  char key = customKeypad.getKey();
  if (key) {
    Serial.print(key);
    tempPassword += key;
    
    if (tempPassword.length() == 4) {
      userPassword = tempPassword;
      passwordSet = true;
      tempPassword = ""; // Limpia la variable temporal para su próximo uso
      Serial.println("\nClave establecida. Por favor, seleccione el modo: A para Chime, B para Alarma.");
    }
  }
}

// Modifica la función enterPassword para que compare contra userPassword en lugar de correctPassword
void enterPassword() {
  char key = customKeypad.getKey();
  if (key) {
    tempPassword += key;
    Serial.print("*"); // Muestra un asterisco por cada tecla presionada

    if (tempPassword.length() == 4) {
      Serial.println(); // Salto de línea tras ingresar 4 dígitos
      
      if (tempPassword == userPassword) {
        systemActive = true; // Activa el sistema
        Serial.println("\nSistema activado.");
        tempPassword = ""; // Restablece la contraseña para una nueva entrada
      } else {
        Serial.println("\nContraseña incorrecta. Intente de nuevo.");
        tempPassword = ""; // Restablece la contraseña para seguir intentando
      }
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
  char key;
  if (tempPassword.length() < 4) {
    key = customKeypad.getKey();
    if (key) {
      tempPassword += key;
      Serial.print("*"); // Muestra un asterisco por cada tecla presionada
    }
  }

  if (tempPassword.length() == 4) {
    if (tempPassword == userPassword) {
      deactivateSystem();
      tempPassword = ""; // Limpia tempPassword para futuras entradas
      return true;
    } else {
      Serial.println("\nContraseña incorrecta. Intente de nuevo.");
      tempPassword = ""; // Limpia tempPassword para reintentar
      // Aquí puede ser necesario llamar a un método para reintentar o mostrar opciones nuevamente
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
    Serial.println("\nModo desactivado. Volviendo al menú principal.");
    Serial.println("Seleccione el modo: A para Chime, B para Alarma.");
    // No es necesario resetear tempPassword aquí ya que se limpia en checkPasswordToDeactivate
}
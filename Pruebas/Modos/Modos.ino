#include <Keypad.h>

const byte ROWS = 4; // Cuatro filas
const byte COLS = 4; // Cuatro columnas
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {39, 8, 7, 6}; // Pines de las filas del teclado a Arduino
byte colPins[COLS] = {5, 4, 3, 32}; // Pines de las columnas del teclado a Arduino
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int ledPin = 13;
int pirPin = 10;
int buzzer = 11;
bool systemActive = false;
bool alarmTriggered = false;
String userPassword = ""; // Nueva variable para almacenar la contraseña del usuario
String tempPassword = ""; // Almacena temporalmente la contraseña mientras se ingresa
unsigned long previousMillis = 0;
unsigned long lastTriggerMillis = 0;
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
  String newPassword = "";
  Serial.println("Ingrese nueva contraseña de 4 dígitos/caracteres:");
  while(newPassword.length() < 4) {
    char key = customKeypad.getKey();
    if (key) {
      newPassword += key;
      Serial.print("*"); // Muestra asteriscos en lugar de los dígitos reales
      delay(100); // Pequeña pausa para evitar entradas duplicadas
    }
  }
  userPassword = newPassword;
  passwordSet = true;
  Serial.println("\nNueva contraseña establecida.");

  Serial.println("\nSeleccione el modo: \nA. Modo Chime \nB. Modo Alarma \nC. Cambiar contraseña ");
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
  if (key) {
    switch(key) {
      case 'A':
      case 'B':
        mode = (key == 'A') ? CHIME : ALARM;
        Serial.println("\nModo seleccionado: " + String(mode == CHIME ? "Chime" : "Alarma"));
        modeSelected = true;
        Serial.println("Ingrese la contraseña para activar el sistema.");
        break;
      case 'C':
        changePassword();
        break;
    }
  }
}

bool enterPasswordForChange() {
  String enteredPassword = "";
  Serial.println("Ingrese contraseña actual:");
  while(enteredPassword.length() < 4) {
    char key = customKeypad.getKey();
    if (key) {
      enteredPassword += key;
      Serial.print("*"); // Para mantener la privacidad de la contraseña
      delay(100); // Pequeña pausa para evitar doble lectura
    }
  }
  Serial.println(); // Salto de línea
  
  if (enteredPassword == userPassword) {
    return true;
  } else {
    Serial.println("Contraseña incorrecta.");
    return false;
  }
}

void changePassword() {
    Serial.println("Cambiando contraseña. Ingrese contraseña actual:");
    while (true) { // Bucle infinito para intentos ilimitados
        if (enterPasswordForChange()) {
            Serial.println("Ingrese nueva contraseña:");
            setPassword(); // Establece la nueva contraseña
            break; // Sale del bucle si la contraseña se cambia correctamente
        } else {
            Serial.println("Contraseña incorrecta. Intente de nuevo:");
            // No es necesario limpiar tempPassword aquí ya que enterPasswordForChange() se encarga
        }
    }
}

//cambiada
void handleSensorAndAlerts() {
  int pirState = digitalRead(pirPin);
  static unsigned long lastTriggerMillis = 0; // Guarda el momento del último disparo de alarma
  
  // Detecta movimiento
  if (pirState == HIGH) {
    Serial.println("Movimiento detectado en la zona.");
    if (mode == CHIME) {
      // En Modo Chime, activa el buzzer solo si previamente no estaba activado
      if (!alarmTriggered) {
        digitalWrite(buzzer, HIGH); // Activa el buzzer
        alarmTriggered = true;
      }
    } else if (mode == ALARM && !alarmTriggered) {
      // En Modo Alarma, inicia la alarma
      alarmTriggered = true;
      digitalWrite(buzzer, HIGH); // Activa el buzzer
      lastTriggerMillis = millis(); // Actualiza el momento del último disparo para el modo Alarma
    }
  } else {
    // No detecta movimiento
    if (mode == CHIME && alarmTriggered) {
      digitalWrite(buzzer, LOW); // Apaga el buzzer inmediatamente en modo Chime
      alarmTriggered = false; // Restablece para permitir nuevas detecciones
    }
  }
  
  // Para el Modo Alarma, maneja el parpadeo del LED y el buzzer continuo
  if (mode == ALARM && alarmTriggered) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      digitalWrite(ledPin, !digitalRead(ledPin)); // Parpadeo del LED
    }
    // No apaga el buzzer en modo Alarma aquí, porque queremos mantenerlo sonando hasta que se desactive la alarma
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
    Serial.println("\nSeleccione el modo: \nA. Modo Chime \nB. Modo Alarma \nC. Cambiar contraseña ");
}

/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID           "TMPL2Zq6nuALv"
#define BLYNK_TEMPLATE_NAME         "Quickstart Template"
#define BLYNK_AUTH_TOKEN            "U_ALVfVlWF2MeTODb25IRA05N19MKWcI"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
// Configuración del teclado
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configuración de los pines
int ledPin = 13;
int pirPin = 10;
int buzzer = 12;
bool systemActive = false;
bool alarmTriggered = false;
String userPassword = ""; // Inicialmente vacía, se establecerá a través de Blynk o serial
String tempPassword = "";
enum OperationMode { NONE, CHIME, ALARM } mode = NONE;
bool modeSelected = false;
bool passwordSet = false;

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "APARTAMENTOS#1#2#3#4#5";
char pass[] = "alfa12345";

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buzzer, OUTPUT);
  Serial.println("Bienvenido al sistema de seguridad. Por favor, configure su sistema a través de la aplicación Blynk.");
}

void loop() {
  Blynk.run();
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

BLYNK_WRITE(V0) { // Función para cambiar/establecer la contraseña desde Blynk
  userPassword = param.asStr();
  passwordSet = true;
  Serial.println("Contraseña establecida/cambiada con éxito.");
}

BLYNK_WRITE(V1) { // Función para activar/desactivar el sistema desde Blynk
  systemActive = param.asInt();
  if (systemActive) {
    Serial.println("Sistema activado desde Blynk.");
  } else {
    Serial.println("Sistema desactivado desde Blynk.");
    deactivateSystem();
  }
}

BLYNK_WRITE(V2) { // Función para seleccionar el modo desde Blynk
  int selectedMode = param.asInt();
  switch(selectedMode) {
    case 1:
      mode = CHIME;
      Serial.println("Modo Chime activado.");
      break;
    case 2:
      mode = ALARM;
      Serial.println("Modo Alarma activado.");
      break;
  }
  modeSelected = true;
}

// Definición de los pines
int ledPin = 13;  // LED conectado al pin 13
int pirPin = 7;   // Pin de entrada para el sensor PIR HC-SR501
int buzzer = 12;  // Pin conectado al buzzer activo

// Variable para almacenar el estado del sensor PIR
int pirState = LOW;             // Inicialmente, no se detecta movimiento

void setup() {
  pinMode(ledPin, OUTPUT);      // Configura el pin del LED como salida
  pinMode(pirPin, INPUT);       // Configura el pin del sensor PIR como entrada
  pinMode(buzzer, OUTPUT);      // Configura el pin del buzzer como salida

  digitalWrite(ledPin, LOW);    // Asegura que el LED está apagado al inicio
}

void loop() {
  pirState = digitalRead(pirPin); // Lee el estado del sensor PIR

  if (pirState == HIGH) {        // Comprueba si el sensor PIR detecta movimiento
    digitalWrite(ledPin, HIGH);  // Enciende el LED
    digitalWrite(buzzer, HIGH);  // Activa el buzzer
    delay(1000);                 // Mantiene el LED y el buzzer activados por 1 segundo
    digitalWrite(buzzer, LOW);   // Apaga el buzzer después de 1 segundo
    // Nota: El LED permanece encendido mientras se detecte movimiento
  } else {
    digitalWrite(ledPin, LOW);   // Apaga el LED si no hay movimiento
    // El buzzer ya está apagado en este punto
  }

  // Opcional: delay para evitar múltiples activaciones en corto tiempo
  delay(100);                    // Pequeño retardo para estabilidad
}

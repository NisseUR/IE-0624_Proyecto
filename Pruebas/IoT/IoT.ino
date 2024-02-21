#include <PubSubClient.h>
#include <WiFi.h>

const char *ssid = "your-ssid";
const char *password = "your-password";
const char *mqttServer = "demo.thingsboard.io";
const int mqttPort = 1883;
const char *device_id = "your-device-id";
const char *access_token = "your-access-token";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    client.setServer(mqttServer, mqttPort);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }

    // Your main code here

    client.loop();
}

void reconnect() {
    while (!client.connected()) {
        Serial.println("Connecting to ThingsBoard MQTT...");

        if (client.connect(device_id, access_token, NULL)) {
            Serial.println("Connected to ThingsBoard MQTT");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Try again in 5 seconds");
            delay(5000);
        }
    }
}

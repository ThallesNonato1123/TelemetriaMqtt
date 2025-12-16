#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17

const char* ssid = "frio";
const char* password = "thalles33";

const char* mqtt_broker = "7a57a721106b42079152200ee7b9345e.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "eusouthalles";
const char* mqtt_password = "Eusouthalles123";

const char* topic_can = "can/signals";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// Variáveis temporárias
double battery = 0;
double temp = 0;
double rpm = 0;

long previous_time = 0;

void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port);
}

void reconnect() {
  while (!mqttClient.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Conectado ao MQTT!");
    } else {
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  wifiClient.setInsecure(); // só para teste
  setupMQTT();
}

void loop() {
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop();

  // Recebe do Nano
  while (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n');
    msg.trim();

    int sepIndex = msg.indexOf(':');
    if (sepIndex > 0) {
      String key = msg.substring(0, sepIndex);
      double value = msg.substring(sepIndex + 1).toDouble();

      if (key == "battery") battery = value;
      else if (key == "temp") temp = value;
      else if (key == "rpm") rpm = value;
    }
  }

  // Publica JSON a cada 500ms
  long now = millis();
  if (now - previous_time > 500) {
    previous_time = now;

    String json = "{";
    json += "\"battery\":" + String(battery,2) + ",";
    json += "\"temp\":" + String(temp,2) + ",";
    json += "\"rpm\":" + String(rpm,0);
    json += "}";

    mqttClient.publish(topic_can, json.c_str());
    Serial.println("JSON enviado: " + json);
  }
}

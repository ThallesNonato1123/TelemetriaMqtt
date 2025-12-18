#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define RXD2 16
#define TXD2 17

// ===== WIFI =====
const char* ssid = "frio";
const char* password = "thalles33";

// ===== MQTT =====
const char* mqtt_broker = "7a57a721106b42079152200ee7b9345e.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;
const char* mqtt_username = "eusouthalles";
const char* mqtt_password = "Eusouthalles123";
const char* topic_can = "can/signals";

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// ===== CONTROLE =====
unsigned long lastPublish = 0;

// ===== MQTT SETUP =====
void setupMQTT() {
  mqttClient.setServer(mqtt_broker, mqtt_port);
}

void reconnectMQTT() {
  while (!mqttClient.connected()) {
    String clientId = "ESP32-";
    clientId += String(random(0xffff), HEX);

    Serial.print("Conectando MQTT... ");
    if (mqttClient.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.println("falhou, tentando novamente...");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // ===== WIFI =====
  Serial.print("Conectando WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // ===== MQTT =====
  wifiClient.setInsecure(); // teste
  setupMQTT();
}

void loop() {
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();

  // ===== LÊ JSON DO ARDUINO =====
  if (Serial2.available()) {
    String json = Serial2.readStringUntil('\n');
    json.trim();

    // Verificação simples de JSON
    if (json.startsWith("{") && json.endsWith("}")) {

      // Publica imediatamente
      mqttClient.publish(topic_can, json.c_str());

      Serial.println("JSON recebido e enviado:");
      Serial.println(json);
    } else {
      Serial.println("Linha inválida recebida:");
      Serial.println(json);
    }
  }
}

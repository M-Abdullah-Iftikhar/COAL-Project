#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

HardwareSerial SerialPort(2);

#define DATABASE_URL "https://coalfinalproject-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define API_KEY "AIzaSyB2pup-1NRpSCpKzEF7mCfRQxmP-AlctlI"
#define SSID "Mughal Raj"
#define PASSWORD "123456789"
#define USER_EMAIL "muhammadabdullahmughal740@gmail.com"
#define USER_PASSWORD "12345678"

const char* mqttServer = "broker.mqtt.cool";
const int mqttPort = 1883;
String receivedValue = "-1";
String uid;

WiFiClient espClient;
WiFiClient thinkClient;
PubSubClient client(espClient);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  SerialPort.begin(9600, SERIAL_8N1, 17, 16);
  connectToWiFi();
  connectToMQTT();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.reconnectWiFi(true);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);
}
void connectToWiFi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT() {
  client.setServer(mqttServer, mqttPort);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("2022-CS-35/esp/esp-controller")) {
      Serial.println("Connected to MQTT");
    } else {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
  client.subscribe("2022-CS-35/esp/textInput");
  client.setCallback(callback);
}

void writeToFireBase(String text) {
    if (Firebase.ready()) {
    Firebase.RTDB.setString(&fbdo, "/Texts/Received", text);
  }
} 

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = topic;
  String payloadStr(reinterpret_cast<char*>(payload), length);
  Serial.println(topicStr);
  Serial.println(payloadStr);
  if (length > 16) {
    payloadStr = " Invalid Input";
  }
  SerialPort.write(0x01);
  delay(20);
  for (int i = 0; i < payloadStr.length(); i++) {
    SerialPort.print(payloadStr.charAt(i));
    Serial.print(payloadStr.charAt(i));
    delay(20);
  }
  Serial.println();
  writeToFireBase(payloadStr);
}

void loop() {
  client.loop();
}

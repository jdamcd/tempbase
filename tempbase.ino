#include <WiFi101.h>
#include <ArduinoJson.h>
#include <DHT.h>

// WiFi
#define SSID ""
#define PASSWORD ""
WiFiSSLClient client;
int status = WL_IDLE_STATUS;

// Firebase
#define HOST ""
#define SECRET ""
#define PATH ""
char bodyBuffer[100];

// Sensor
#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LED 6
#define INTERVAL 20 * 60 * 1000 // 20 mins

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  // while (!Serial);

  connectWiFi();
  syncRtc();
  Serial.println("RTC sync complete");
}

void connectWiFi() {
  status = WiFi.status();
  while (status != WL_CONNECTED) {
    Serial.println("Connecting to WiFi");
    status = WiFi.begin(SSID, PASSWORD);
  }
  Serial.println("WiFi connected");
}

void loop() {
  digitalWrite(LED, HIGH);

  connectWiFi();
  client.connect(HOST, 443);
  delay(1000);
  if (!client.connected()) {
    Serial.println("Error connecting to host");
  } else {
    Serial.println("Measuring");
    int temp = dht.readTemperature();
    int humidity = dht.readHumidity();
    sendData(temp, humidity);
    client.stop();
  }

  WiFi.disconnect();
  digitalWrite(LED, LOW);
  delay(INTERVAL);
}

void sendData(int temp, int humidity) {
  Serial.println("Preparing payload:");
  int size = constructJson(getEpoch(), temp, humidity);
  Serial.println(bodyBuffer);

  char pathBuffer[100];
  sprintf(pathBuffer, "POST %s?auth=%s HTTP/1.1", PATH, SECRET);
  client.println(pathBuffer);
  client.print("Host: ");
  client.println(HOST);
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(size);
  client.println("Accept: application/json");
  client.println("Connection: close");
  client.println();
  client.println(bodyBuffer);

  printResponse();
}

void printResponse() {
  delay(1000);
  int readSize = client.available();
  while (readSize > 0) {
    Serial.print((char) client.read());
    readSize--;
  }
}

int constructJson(unsigned long timestamp, int temperature, int humidity) {
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["timestamp"] = timestamp;
  root["temperature"] = temperature;
  root["humidity"] = humidity;
  root.printTo(bodyBuffer, sizeof(bodyBuffer));
  return root.measureLength();
}


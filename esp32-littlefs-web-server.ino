#include <LittleFS.h>
#include <WiFi.h>
#include <WebServer.h>
#include "FS.h"
#include <ArduinoJson.h>

WebServer server(80);

String wifi_ssid = "";
String wifi_password = "";

void loadConfig() {
  if (LittleFS.exists("/config.json")) {
    File file = LittleFS.open("/config.json", "r");
    StaticJsonDocument<512> doc;

    DeserializationError error = deserializeJson(doc, file);
    if (error) {
      Serial.println("Failed to read config file");
      return;
    }

    wifi_ssid = doc["wifi_ssid"].as<String>();
    wifi_password = doc["wifi_password"].as<String>();

    Serial.println("WiFi SSID: " + wifi_ssid);
    Serial.println("WiFi Password: " + wifi_password);
    file.close();
  } else {
    Serial.println("Config file not found");
  }
}

void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (file) {
    server.streamFile(file, "text/html");
    file.close();
  } else {
    server.send(404, "text/plain", "File not found");
  }
}

void setup() {
  Serial.begin(115200);

  if (!LittleFS.begin(true)) {
    Serial.println("Failed to mount LittleFS");
    return;
  }

  Serial.println("LittleFS mounted successfully");

  loadConfig();

  if (wifi_ssid != "" && wifi_password != "") {
    WiFi.begin(wifi_ssid.c_str(), wifi_password.c_str());
    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }

    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Invalid WiFi credentials");
  }

  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

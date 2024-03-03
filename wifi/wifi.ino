
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <Arduino_JSON.h>

const char *ssid = "mega";
const char *password = "12345678";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
JSONVar readings;

unsigned long lastTime = 0;
unsigned long timerDelay = 500;

String value1 = "value1";
String value2 = "value2";
String value3 = "value3";
String value4 = "value4";
String value5 = "value5";
String value6 = "value6";
String value7 = "value7";
String value8 = "value8";

void initFS() {
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  } else {
    Serial.println("LittleFS mounted successfully");
  }
}

void initWiFi() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("Access Point IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char *)data;
    // Check if the message is "getReadings"
    if (strcmp((char *)data, "getReadings") == 0) {
      // if it is, send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    }
    if (strcmp((char *)data, "toggle") == 0) {
      Serial.println('!');
    }
    if (strcmp((char *)data, "fan") == 0) {
      Serial.println('~');
    }
  }
}


void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initFS();
  initWebSocket();

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html; charset=UTF-8");
  });
  server.on("/iran.woff", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/iran.woff", "application/font-woff");
  });

  server.serveStatic("/", LittleFS, "/");

  server.begin();
}

void loop() {
  if (Serial.available() > 0) {
    char firstChar = Serial.read();  // خواندن اولین کاراکتر

    // اگر اولین کاراکتر برابر با علامت سوال (?) باشد
    if (firstChar == '?') {
      String input = Serial.readStringUntil('\n');

      int comma1 = input.indexOf(',');
      int comma2 = input.indexOf(',', comma1 + 1);
      int comma3 = input.indexOf(',', comma2 + 1);
      int comma4 = input.indexOf(',', comma3 + 1);
      int comma5 = input.indexOf(',', comma4 + 1);
      int comma6 = input.indexOf(',', comma5 + 1);
      int comma7 = input.indexOf(',', comma6 + 1);

      value1 = input.substring(0, comma1);
      value2 = input.substring(comma1 + 1, comma2);
      value3 = input.substring(comma2 + 1, comma3);
      value4 = input.substring(comma3 + 1, comma4);
      value5 = input.substring(comma4 + 1, comma5);
      value6 = input.substring(comma5 + 1, comma6);
      value7 = input.substring(comma6 + 1, comma7);
      value8 = input.substring(comma7 + 1);
    }
  }
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    notifyClients(sensorReadings);
    lastTime = millis();
  }
  ws.cleanupClients();
}

String getSensorReadings() {
  readings["temperature"] = value1;
  readings["humidity"] = value2;
  readings["soil"] = value3;
  readings["text"] = value4;
  readings["PIR_STATE"] = value5;
  readings["PIR_WORK"] = value6;
  readings["fan_auto"] = value7;
  readings["fan_state"] = value8;

  String jsonString = JSON.stringify(readings);
  return jsonString;
}

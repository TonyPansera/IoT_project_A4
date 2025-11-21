#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Heltec_AP_Tony";
const char* password = "12345678";

WebServer server(80);

// Exemple de valeur à afficher
float sensorValue = 42.7;

void handleRoot() {
  String page = "<html><body><h1>Valeurs Heltec</h1>";
  page += "<p>Valeur = " + String(sensorValue) + "</p>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void setup() {
  Serial.begin(115200);

  // Mode point d'accès
  WiFi.softAP(ssid, password);

  // Endpoints HTTP
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}

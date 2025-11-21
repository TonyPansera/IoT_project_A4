#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>

const char* ssid = "Heltec_AP_Tony";
const char* password = "12345678";

WebServer server(80);

// Exemple de valeur à afficher (utilisé si tu veux ajouter un endpoint JSON)
float sensorValue = 42.7;

// --- Handlers pour les fichiers ---
void handleRoot() {
  File file = LittleFS.open("../web/index.html", "r");
  if (!file) {
    server.send(500, "text/plain", "index.html not found");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

void handleCSS() {
  File file = LittleFS.open("../web/style.css", "r");
  if (!file) {
    server.send(404, "text/plain", "style.css not found");
    return;
  }

  server.streamFile(file, "text/css");
  file.close();
}

void setup() {
  Serial.begin(115200);

  // --- Mount LittleFS ---
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS mount failed");
    while (true) { delay(1000); }
  }
  Serial.println("LittleFS mounted OK");

  // --- WiFi AP ---
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  // --- Routes ---
  server.on("/", handleRoot);
  server.on("/style.css", handleCSS);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

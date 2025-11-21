#include <WiFi.h>
#include <WebServer.h>
#include "web/style.h"
#include "web/index.h"

// =======================
// WIFI — Mode AP uniquement
// =======================
const char* ssid     = "Heltec_AP_Tony";
const char* password = "12345678";

WebServer server(80);

// =======================
// DONNÉES (exemple)
// =======================
float temperature = 23.7f;
float humidity    = 54.2f;

// =======================
// TEMPLATE ENGINE
// =======================
String processTemplate(float temp, float hum) {
    String html = index_html;  // index_html = ton fichier PROGMEM
    html.replace("%TEMP%", String(temp, 1));
    html.replace("%HUM%",  String(hum, 1));
    return html;
}

// =======================
// HANDLERS HTTP
// =======================
void handleIndex() {
    String page = processTemplate(temperature, humidity);
    server.send(200, "text/html", page);
}

void handleCSS() {
    server.send_P(200, "text/css", style_css);  // style_css = ton CSS PROGMEM
}

void handleNotFound() {
    server.send(404, "text/plain", "404 Not found");
}

// =======================
// SETUP
// =======================
void setup() {
    Serial.begin(115200);
    delay(200);

    // WiFi AP uniquement
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    Serial.println("=== WiFi AP mode ===");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("IP:   ");
    Serial.println(WiFi.softAPIP());

    // Routes HTTP
    server.on("/", handleIndex);
    server.on("/style.css", handleCSS);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server ready");
}

// =======================
// LOOP
// =======================
void loop() {
    server.handleClient();

    // Mise à jour des valeurs (simulation)
    static uint32_t t0 = 0;
    if (millis() - t0 > 2000) {
        t0 = millis();
        temperature += 0.1f;  // remplace par tes vraies mesures capteur
        humidity    += 0.2f;
    }
}

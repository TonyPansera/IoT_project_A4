/*
  Code Récepteur pour Heltec V3
*/
#include <WiFi.h>
#include <WebServer.h>
#include "web/style.h"
#include "web/index.h"
#include <SPI.h>
#include "LoRaWan_APP.h"

// CONFIGURATION PINS HELTEC V3
#define V3_SCK   9
#define V3_MISO  11
#define V3_MOSI  10
#define V3_SS    8
#define V3_RST   12

// CONFIGURATION LORA
#define RF_FREQUENCY            868000000 
#define LORA_BANDWIDTH          0
#define LORA_SPREADING_FACTOR   7
#define LORA_CODINGRATE         1
#define LORA_PREAMBLE_LENGTH    8
#define LORA_SYMBOL_TIMEOUT     0
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON    false

// CONFIGURATION BUZZER
#define buzzer 46

// CONFIGURATION RADIO
static RadioEvents_t RadioEvents;
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

// CONFIGURATION WIFI
const char* ssid     = "Heltec_AP_Tony";
const char* password = "12345678";

WebServer server(80);

// variables utilisées
float temperature = 0.0;
float humidity    = 0.0;
float pression = 0.0;
int gas_value = 0;
int rx_count = 0;
int lux_value = 0;
int hb_value = 0;
unsigned long lastRxTime = 0; 

// Update des données reçues sur le site internet
String processTemplate(float temp, float hum, float press) {
    String html = index_html; 
    html.replace("%TEMP%", String(temp, 1));
    html.replace("%HUM%",  String(hum, 1));
    html.replace("%PRES%",  String(press, 1));
    html.replace("%GAS%",  String(gas_value));
    html.replace("%LUX%",  String(lux_value));
    html.replace("%BPM%",  String(hb_value));

    // temps en seconde depuis les dernières données reçues
    unsigned long secondsAgo = 0;
    
    if (lastRxTime > 0) {
        secondsAgo = (millis() - lastRxTime) / 1000;
    }
    
    html.replace("%TIME%", String(secondsAgo));

    return html;
}

void handleIndex() {
    String page = processTemplate(temperature, humidity, pression);
    server.send(200, "text/html", page);
}

void handleCSS() {
    server.send_P(200, "text/css", style_css); 
}

void handleNotFound() {
    server.send(404, "text/plain", "404 Not found");
}


void setup() {
    Serial.begin(115200);
    delay(500);

    // Initialisation du SPI (Heltec)
    SPI.begin(V3_SCK, V3_MISO, V3_MOSI, V3_SS);

    // Reset de la radio
    pinMode(V3_RST, OUTPUT);
    digitalWrite(V3_RST, LOW); delay(20);
    digitalWrite(V3_RST, HIGH); delay(50);

    // Initialisation de la radio
    RadioEvents.RxDone = OnRxDone;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                   LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                   LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   0, true, 0, 0, LORA_IQ_INVERSION_ON, true );
    
    // Mode écoute continue
    Radio.Rx(0);

    // Configuration du WiFi
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, NULL, 1, 0, 4);

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

    // Initialisation du buzzer
    pinMode(buzzer, OUTPUT);
}


void loop() {
    // Gestion de la radio
    Radio.IrqProcess();

    // Gestion du serveur web
    server.handleClient();
    delay(100);
}


// Lorsque des données sont reçues
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ) {

    char rxBuffer[100];
    
    // On copie les données reçues dans le buffer
    memcpy(rxBuffer, payload, size);

    // IMPORTANT : On ajoute le caractère de fin de chaîne '\0'
    rxBuffer[size] = '\0'; 

    Serial.println("-----------------------------");
    Serial.print("Message brut recu: ");
    Serial.println(rxBuffer);

    // Le format doit correspondre à celui du sprintf de l'émetteur
    int n = sscanf(rxBuffer, "T:%f H:%f P:%f G:%d L:%d HB:%d C:%d", 
                   &temperature, &humidity, &pression, &gas_value, &lux_value, &hb_value, &rx_count);

    if (n == 7) { // Si sscanf a réussi à trouver les 7 variables
        Serial.println("Decodage réussi !");
        Serial.print(" >> Temperature : "); Serial.println(temperature);
        Serial.print(" >> Humidite    : "); Serial.println(humidity);
        Serial.print(" >> Pression    : "); Serial.println(pression);
        Serial.print(" >> Gaz (MQ5)   : "); Serial.println(gas_value);
        Serial.print(" >> Lumiere    : "); Serial.println(lux_value);
        Serial.print(" >> BPM    : "); Serial.println(hb_value);
        Serial.print(" >> Compteur    : "); Serial.println(rx_count);

        // Activation du buzzer si une limite est dépassée
        if(gas_value > 450) {
            digitalWrite(buzzer, HIGH);
        } else {
            digitalWrite(buzzer, LOW);
        }

        lastRxTime = millis(); 

    } else {
        Serial.println("Erreur de formatage du message.");
    }

    Serial.print("RSSI: "); Serial.println(rssi); // Force du signal

    // On relance l'écoute pour le prochain message
    Radio.Rx(0);
}

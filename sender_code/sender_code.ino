/*
  Code Émetteur pour Heltec V3
*/
#include "HT_SSD1306Wire.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BME680.h>
#include "LoRaWan_APP.h"
#include <Arduino.h>
#include "Seeed_TMG3993.h"


// CONFIGURATION PINS HELTEC V3
#define V3_SCK   9
#define V3_MISO  11
#define V3_MOSI  10
#define V3_SS    8
#define V3_RST   12
#define V3_DIO1  14

// CONFIGURATION PINS BME680 (en SPI)
#define BME_SCK  36 // SCL du capteur
#define BME_MISO 37 // SDO du capteur
#define BME_MOSI 35  // SDA du capteur
#define BME_CS   34  // CS  du capteur

// CONFIGURATION LORA
#define RF_FREQUENCY                    868000000 
#define TX_OUTPUT_POWER                 14        
#define LORA_BANDWIDTH                  0         
#define LORA_SPREADING_FACTOR           7         
#define LORA_CODINGRATE                 1         
#define LORA_PREAMBLE_LENGTH            8         
#define LORA_SYMBOL_TIMEOUT             0         
#define LORA_FIX_LENGTH_PAYLOAD_ON      false
#define LORA_IQ_INVERSION_ON            false

// CONFIGURATION TMG39931 (en I2C)
#define TMG_SDA  41
#define TMG_SCL  42
#define TMG_ADDR 0x39

// CONFIGURATION AUTRES CAPTEURS
#define sensor 7
#define HB_PIN 1


// Variables utilisées dans le code
unsigned int counter =  0; // Nombre d'itérations
float temperature =     0.0;
float humidity =        0.0;
float pression =        0.0;
int gas_value =         0;
int32_t lux_value =     0;

bool flag = true; // Pour gérer l'affichage de l'écran


// Création des différents objets utilisés
SSD1306Wire  ecran(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);
Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);
TMG3993 tmg(TMG_ADDR);
bool tmgFound = true;

static RadioEvents_t RadioEvents;
void OnTxDone( void );
void OnTxTimeout( void );


void setup() {
    Serial.begin(115200);
    delay(500);

    // Initialisation de l'écran
    ecran.init();
    ecran.setFont(ArialMT_Plain_10);
    ecran.setTextAlignment(TEXT_ALIGN_LEFT);
    ecran.clear();
    ecran.drawString(0, 0, "Heltec V3 Sender");
    ecran.drawString(0, 15, "Initialisation...");
    ecran.display();

    // Initialisation du bus I2C secondaire
    Wire1.begin(TMG_SDA, TMG_SCL);

    delay(1000);

    // Initialisation du capteur TMG3993
    if (tmg.initialize()) {
        Serial.println("TMG3993 detecte !");
        tmgFound = true;
        tmg.setADCIntegrationTime(0xdb);
        tmg.enableEngines(ENABLE_PON | ENABLE_AEN);
        
    } else {
        Serial.println("Erreur TMG3993 (Wire)");
        ecran.drawString(0, 25, "Err: TMG3993");
        tmgFound = false;
    }

    // Configuration du SPI
    SPI.begin(V3_SCK, V3_MISO, V3_MOSI, V3_SS);

    // Reset de la radio (au cas-où)
    pinMode(V3_RST, OUTPUT);
    digitalWrite(V3_RST, LOW);
    delay(20);
    digitalWrite(V3_RST, HIGH);
    delay(50);

    // Initialisation de la radio
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    
    // On donne les pins explicites (au cas-où)
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );
                                   
    // Initialisation du capteur BME680
    if (!bme.begin()) {
        Serial.println("BME680 SPI introuvable ! Vérifie le cablage.");
        ecran.drawString(0, 15, "Erreur BME (SPI)");
        ecran.display();
    } else {
        bme.setTemperatureOversampling(BME680_OS_8X);
        bme.setHumidityOversampling(BME680_OS_2X);
        bme.setPressureOversampling(BME680_OS_4X);
        bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    }

    // Initialisation du capteur de gaz
    pinMode(sensor, INPUT);

    Serial.println("MQ5 Heating Up!");
    delay(1000); // normalement le capteur a besoin d'un peu de temps pour chauffer et fonctionner correctement
}

void loop() {

    // On vérifie si la radio a fini sa tâche précédente
    Radio.IrqProcess(); 

    // Lecture du BME680
    if (bme.performReading()) {
        temperature = bme.temperature;
        humidity = bme.humidity;
        pression = bme.pressure / 100.0;
    } else {
        Serial.println("Erreur lecture BME680");
    }

    // Lecture du capteur de battements de coeur
    int adcValue = analogReadMilliVolts(HB_PIN);

    // Lecture du TMG3993
    if (tmgFound) {
        uint8_t status = tmg.getSTATUS();

        // On vérifie si une donnée pour la lumière est valide (AVALID bit 0)
        if (status & 0x01) { 
            uint16_t r, g, b, c;
            tmg.getRGBCRaw(&r, &g, &b, &c); // Lecture des données brutes
            
            // Calcul des Lux
            lux_value = tmg.getLux(r, g, b, c);
            Serial.print("Lux: "); Serial.println(lux_value);
        
            tmg.clearALSInterrupts(); 
        } else {
            Serial.println("TMG : Donnée non prête (Status AVALID = 0)");
        }
    } else {
        lux_value = -1;
    }

    // Lecture du MQ5
    gas_value = analogRead(sensor);

    // Mise à jour de l'écran
    ecran.clear();
    ecran.setFont(ArialMT_Plain_10);
    ecran.drawString(0, 0, "Heltec V3 Sender #" + String(counter));

    if(flag) {
        ecran.drawString(0, 15, "Temp: " + String(temperature, 1) + " C");
        ecran.drawString(0, 27, "Hum : " + String(humidity, 1) + " %");
        ecran.drawString(0, 39, "Pres: " + String(pression, 0) + " hPa");
    } else {
        ecran.drawString(0, 15, "Gaz: " + String(gas_value) + " ppm"); 
        ecran.drawString(0, 27, "Light: " + String(lux_value) + " unit"); 
        ecran.drawString(0, 39, "HB: " + String(adcValue) + " bpm"); 
    }
    ecran.display(); 
    flag = !flag;

    // Envoi du message
    char message[100]; 
    sprintf(message, "T:%.2f H:%.2f P:%.1f G:%d L:%d HB:%d C:%d", 
            temperature, humidity, pression, gas_value, lux_value, adcValue, counter);
    Serial.print("Envoi LoRa: "); 
    Serial.println(message);
    
    Radio.Send( (uint8_t *)message, strlen(message) );

    counter++;

    delay(1000);
}

void OnTxDone( void ) {
    Serial.println(" -> SUCCES (TxDone) !");
    Radio.Sleep();
}

void OnTxTimeout( void ) {
    Serial.println(" -> ECHEC (Timeout)");
    Radio.Sleep();
}
# 2× Heltec WiFi LoRa 32 V3 : LoRa entre elles + appli web en Wi-Fi

Objectif de cette partie : faire un mini système IoT avec **deux Heltec LoRa 32 V3**.  
- **Node A** lit des capteurs et envoie les mesures en **LoRa**.  
- **Node B** reçoit en LoRa et affiche les dernières valeurs sur une **page web via Wi-Fi**. :contentReference[oaicite:0]{index=0}

---

## Ce que j’ai fait

### Node A (Sender)
- Lecture des capteurs (ex : BME680 → température / humidité / pression, ou ceux testés avant).
- Formatage d’un payload simple.
- Envoi périodique sur le lien **LoRa SX1262**.

### Node B (Receiver + Web server)
- Réception des paquets LoRa.
- Parsing + stockage des dernières valeurs reçues.
- Connexion au Wi-Fi.
- Lancement d’un serveur HTTP qui :
  - sert une page `index.html`
  - expose un endpoint `/data` en JSON pour la web app.

---

## Matos
- 2× Heltec **WiFi LoRa 32 V3 (ESP32-S3 + SX1262)**
- Capteurs sur Node A
- 2 antennes LoRa
- **Câbles USB-C → USB-A obligatoires** (les câbles USB-C→USB-C ne marchent pas bien avec ces cartes). 

---

## Dépendances
Dans Arduino IDE :
- Board package Heltec ESP32 (quick start Heltec).
- La lib LoRa/SX1262 utilisée au TP (ex : Heltec LoRa lib ou RadioLib).
- Côté serveur Wi-Fi : `WiFi.h` + `WebServer.h` (ou `ESPAsyncWebServer` si version async).



#pragma once
#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Station Heltec</title>
    <link rel="stylesheet" href="/style.css">

    <!-- Auto-refresh every 2 seconds -->
    <meta http-equiv="refresh" content="2">
</head>
<body>

    <header>
        <h1>Station Environnementale</h1>
        <p>Heltec LoRa 32 V3 – Monitoring</p>
    </header>

    <main>

        <div class="card">
            <h2>🌡️ Température</h2>
            <p class="value">%TEMP%</p>
            <span class="unit">°C</span>
        </div>

        <div class="card">
            <h2>💧 Humidité</h2>
            <p class="value">%HUM%</p>
            <span class="unit">%</span>
        </div>

    </main>

    <footer>
        <p>Heltec ESP32-S3 • Web Dashboard</p>
    </footer>

</body>
</html>
)rawliteral";
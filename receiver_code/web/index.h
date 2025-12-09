#pragma once
#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
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
        <h1>Sensor station</h1>
        <p>Heltec LoRa 32 V3 – Monitoring</p>
    </header>

    <main>

        <div class="card">
            <h2>🌡️ Temperature</h2>
            <p class="value">%TEMP%</p>
            <span class="unit">°C</span>
        </div>

        <div class="card">
            <h2>💧 Humidity</h2>
            <p class="value">%HUM%</p>
            <span class="unit">%</span>
        </div>

        <div class="card">
            <h2>💪 Pressure </h2>
            <p class="value">%PRES%</p>
            <span class="unit">hPa</span>
        </div>

        <div class="card">
            <h2>💨 Gas </h2>
            <p class="value">%GAS%</p>
            <span class="unit">ppm</span>
        </div>

        <div class="card">
            <h2>💡 Light </h2>
            <p class="value">%LUX%</p>
            <span class="unit">ppm</span>
        </div>

        <div class="card">
            <h2>💓 BPM </h2>
            <p class="value">%BPM%</p>
            <span class="unit">bpm</span>
        </div>

        <div class="status-bar">
            ⏳ Dernier paquet reçu il y a : <strong>%TIME%</strong> secondes
        </div>

    </main>

    <footer>
        <p>Heltec LoRa V3 • Web Dashboard</p>
    </footer>

</body>
</html>
)rawliteral";
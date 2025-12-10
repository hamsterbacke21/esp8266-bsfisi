#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ===== WLAN-DATEN =====
const char* ssid     = "wlan1313";
const char* password = "wlan1313pw";

// ===== LED-PIN =====
const int LED_PIN = 2;   // eingebaute LED (D4)

// ===== DS18B20 – ONE-WIRE-BUS =====
// D1 (GPIO 5) als Daten-Pin für den DS18B20
#define ONE_WIRE_BUS D3   // oder D1, wenn dein Board das Makro kennt

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

// ===== WEBSERVER =====
ESP8266WebServer server(80);

// ===== GLOBAL =====
bool lightState = false;

// ===== TEMPERATUR AUS DS18B20 =====
float getTemperature() {
  DS18B20.requestTemperatures();
  float t = DS18B20.getTempCByIndex(0); // erster Sensor auf dem Bus

  if (t == DEVICE_DISCONNECTED_C || t < -50.0 || t > 125.0) {
    Serial.println("Fehler beim Lesen der Temperatur vom DS18B20!");
    return NAN;
  }

  return t;
}

// ===== HTML-SEITE MIT JS (ALLES IM STRING!) =====
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>SmartDevices – Licht &amp; Temperatur</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      padding: 20px;
      margin: 0;
      background: #0f172a;
      color: #e5e7eb;
      text-align: center;
    }
    .card {
      max-width: 360px;
      margin: 0 auto;
      margin-top: 20px;
      background: #020617;
      border-radius: 16px;
      padding: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.5);
    }
    h1 {
      font-size: 1.4rem;
      margin: 0 0 4px;
    }
    h2 {
      font-size: 0.9rem;
      margin: 0 0 16px;
      color: #9ca3af;
      font-weight: 400;
    }
    .temperature-box {
      background: linear-gradient(135deg, #1d4ed8, #22c55e);
      border-radius: 14px;
      padding: 16px;
      margin-bottom: 18px;
    }
    .temperature-label {
      font-size: 0.85rem;
      opacity: 0.9;
    }
    .temperature-value {
      font-size: 2.0rem;
      font-weight: 600;
    }
    .temperature-unit {
      font-size: 1.0rem;
      opacity: 0.9;
      margin-left: 4px;
    }
    .radio-group {
      background: #020617;
      border: 1px solid #1f2937;
      border-radius: 14px;
      padding: 14px;
      text-align: left;
    }
    .radio-title {
      font-size: 0.9rem;
      margin-bottom: 6px;
    }
    .radio-option {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.95rem;
      margin-bottom: 4px;
    }
    input[type="radio"] {
      width: 18px;
      height: 18px;
      accent-color: #22c55e;
    }
    .status {
      margin-top: 10px;
      font-size: 0.85rem;
      color: #d1d5db;
    }
    .chip {
      display: inline-flex;
      align-items: center;
      gap: 6px;
      font-size: 0.8rem;
      padding: 4px 10px;
      border-radius: 999px;
      background: #111827;
      border: 1px solid #1f2937;
      margin-top: 6px;
    }
    .chip-dot {
      width: 8px;
      height: 8px;
      border-radius: 50%;
      background: #22c55e;
    }
    .chip.off .chip-dot {
      background: #ef4444;
    }
    .footer {
      margin-top: 16px;
      font-size: 0.75rem;
      color: #6b7280;
    }
  </style>
</head>
<body>
  <div class="card">
    <h1>SmartDevices – Raumkontrolle</h1>
    <h2>Lichtsteuerung &amp; Temperaturüberwachung</h2>

    <div class="temperature-box">
      <div class="temperature-label">Aktuelle Temperatur</div>
      <div>
        <span id="temp" class="temperature-value">--.-</span>
        <span class="temperature-unit">°C</span>
      </div>
    </div>

    <div class="radio-group">
      <div class="radio-title">Lichtsteuerung</div>
      <label class="radio-option">
        <input type="radio" name="light" value="on" onchange="toggleLight('on')">
        <span>Licht an</span>
      </label>
      <label class="radio-option">
        <input type="radio" name="light" value="off" onchange="toggleLight('off')">
        <span>Licht aus</span>
      </label>
    </div>

    <div class="status">
      Status: <span id="light-status-text">Unbekannt …</span>
      <div id="chip" class="chip off">
        <span class="chip-dot"></span>
        <span id="chip-label">Licht aus</span>
      </div>
    </div>

    <div class="footer">
      CPS-Frontend · SmartDevices GmbH
    </div>
  </div>

  <script>
    function updateStatus(isOn) {
      const chip = document.getElementById('chip');
      const label = document.getElementById('chip-label');
      const statusText = document.getElementById('light-status-text');
      const radios = document.querySelectorAll('input[name="light"]');

      if (isOn) {
        chip.classList.remove('off');
        label.textContent = 'Licht an';
        statusText.textContent = 'Licht ist eingeschaltet';
      } else {
        chip.classList.add('off');
        label.textContent = 'Licht aus';
        statusText.textContent = 'Licht ist ausgeschaltet';
      }

      radios.forEach(r => {
        r.checked = (r.value === (isOn ? 'on' : 'off'));
      });
    }

    async function toggleLight(state) {
      try {
        const res = await fetch('/led?state=' + state);
        const txt = await res.text();
        const isOn = (txt.trim() === 'on');
        updateStatus(isOn);
      } catch (e) {
        console.error(e);
      }
    }

    async function updateTemperature() {
      try {
        const res = await fetch('/temperature');
        const txt = await res.text();
        if (txt === 'nan') {
          document.getElementById('temp').textContent = '--.-';
        } else {
          document.getElementById('temp').textContent = txt;
        }
      } catch (e) {
        console.error(e);
      }
    }

    // Initiale Updates
    setInterval(updateTemperature, 2000);
    window.onload = () => {
      updateStatus(false);
      updateTemperature();
    };
  </script>
</body>
</html>
)rawliteral";

// ===== ROUTEN =====
void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void handleLed() {
  if (server.hasArg("state")) {
    String s = server.arg("state");

    if (s == "on") {
      digitalWrite(LED_PIN, LOW);    // LED an (invertiert)
      lightState = true;
    } else if (s == "off") {
      digitalWrite(LED_PIN, HIGH);   // LED aus
      lightState = false;
    }
  }

  server.send(200, "text/plain", lightState ? "on" : "off");
}

void handleTemperature() {
  float t = getTemperature();

  if (isnan(t)) {
    server.send(200, "text/plain", "nan");
  } else {
    server.send(200, "text/plain", String(t, 1));
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED aus

  // DS18B20 initialisieren
  DS18B20.begin();
  Serial.println("DS18B20 initialisiert.");

  Serial.print("Verbinde mit WLAN ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nVerbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led", handleLed);
  server.on("/temperature", handleTemperature);

  server.begin();
  Serial.println("Webserver gestartet");
}

void loop() {
  server.handleClient();
}

#include <DallasTemperature.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <PubSubClient.h>

// ===== WLAN-DATEN =====
const char *ssid = "wlan1313";
const char *password = "wlan1313pw";

// ===== LED-PIN =====
const int LED_PIN = 2; // eingebaute LED (D4)

// ===== DS18B20 – ONE-WIRE-BUS =====
// D1 (GPIO 5) als Daten-Pin für den DS18B20
#define ONE_WIRE_BUS D3 // oder D1, wenn dein Board das Makro kennt

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

// ===== MQTT-BROKER =====
const char *mqttServer = "192.168.1.172";
const int mqttPort = 1883;
const char *mqttClientId = "esp8266-smartdevice";

// Topics
const char *topicTemp = "esp8266/temperature";   // publish
const char *topicLedState = "esp8266/led/state"; // publish
const char *topicLedSet = "esp8266/led/set";     // subscribe

// ===== WEBSERVER =====
ESP8266WebServer server(80);

// ===== MQTT-CLIENT =====
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ===== GLOBAL =====
bool lightState = false;
unsigned long lastMqttPublish = 0;
const unsigned long mqttInterval = 1000; // ms between publishes

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

// ===== MQTT CALLBACK (eingehende Nachrichten) =====
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++)
    msg += (char)payload[i];
  msg.trim();

  if (String(topic) == topicLedSet) {
    if (msg == "on") {
      digitalWrite(LED_PIN, LOW); // LED an (invertiert)
      lightState = true;
    } else if (msg == "off") {
      digitalWrite(LED_PIN, HIGH); // LED aus
      lightState = false;
    }
    mqtt.publish(topicLedState, lightState ? "on" : "off", true);
    Serial.print("LED via MQTT gesetzt: ");
    Serial.println(msg);
  }
}

// ===== MQTT VERBINDEN / WIEDERVERBINDEN =====
void mqttReconnect() {
  if (mqtt.connected())
    return;
  Serial.print("Verbinde mit MQTT-Broker … ");
  if (mqtt.connect(mqttClientId)) {
    Serial.println("verbunden!");
    mqtt.subscribe(topicLedSet);
    Serial.print("Abonniert: ");
    Serial.println(topicLedSet);
  } else {
    Serial.print("Fehler, rc=");
    Serial.println(mqtt.state());
  }
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
    .warning {
      margin-top: 8px;
      font-size: 0.9rem;
      color: #f97316;
      font-weight: 600;
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

    <div id="warning" class="warning"></div>

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
      let rickRolled = false;
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

        const tempElem = document.getElementById('temp');
        const warningElem = document.getElementById('warning');

        if (txt === 'nan') {
          tempElem.textContent = '--.-';
          warningElem.textContent = '';
        } else {
          tempElem.textContent = txt;

          const val = parseFloat(txt.replace(',', '.'));

          if (!isNaN(val)) {
            // Warnung ab 29 °C
            if (val >= 29.0) {
              warningElem.textContent = 'Obacht Mike, die Maus ist im Raum!';
            } else {
              warningElem.textContent = '';
            }

            // Rickroll ab 31 °C, nur einmal
            if (val >= 31.0 && !rickRolled) {
              rickRolled = true;
              // kleine Verzögerung, damit man die Anzeige noch sieht
              setTimeout(() => {
                window.location.href = 'https://www.youtube.com/watch?v=dQw4w9WgXcQ';
              }, 1000);
            }
          } else {
            warningElem.textContent = '';
          }
        }
      } catch (e) {
        console.error(e);
      }
    }


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
void handleRoot() { server.send_P(200, "text/html", INDEX_HTML); }

void handleLed() {
  if (server.hasArg("state")) {
    String s = server.arg("state");

    if (s == "on") {
      digitalWrite(LED_PIN, LOW); // LED an (invertiert)
      lightState = true;
    } else if (s == "off") {
      digitalWrite(LED_PIN, HIGH); // LED aus
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

  // MQTT einrichten
  mqtt.setServer(mqttServer, mqttPort);
  mqtt.setCallback(mqttCallback);
  mqttReconnect();

  // Webserver-Routen
  server.on("/", handleRoot);
  server.on("/led", handleLed);
  server.on("/temperature", handleTemperature);

  server.begin();
  Serial.println("Webserver gestartet");
}

void loop() {
  // MQTT verbunden halten
  if (!mqtt.connected())
    mqttReconnect();
  mqtt.loop();

  // Webserver bedienen
  server.handleClient();

  // Temperatur periodisch per MQTT senden
  unsigned long now = millis();
  if (now - lastMqttPublish >= mqttInterval) {
    lastMqttPublish = now;
    float t = getTemperature();
    if (!isnan(t)) {
      String payload = String(t, 1);
      mqtt.publish(topicTemp, payload.c_str());
      Serial.print("MQTT veröffentlicht [esp8266/temperature]: ");
      Serial.println(payload);
    } else {
      Serial.println("Temperaturlesefehler – nichts veröffentlicht.");
    }
  }
}

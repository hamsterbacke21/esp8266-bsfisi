#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// ===== WLAN-DATEN ANPASSEN =====
const char* ssid = "wlan1313";
const char* password = "wlan1313pw";

// ===== LED-PIN =====
// NodeMCU: eingebaute LED = GPIO2 (D4)
const int LED_PIN = 2;

// ===== Webserver =====
ESP8266WebServer server(80);

// ===== Globale Variablen =====
bool lightState = false;

// ===== Temperatur-Funktion (DUMMY) =====
// Wird später durch echten Sensor ersetzt
float getTemperature() {
  return 24.7;
}

// ===== HTML-SEITE =====
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SmartDevices Steuerung</title>
<style>
  body { font-family: Arial; padding: 20px; text-align: center; }
  .btn { padding: 14px; margin: 8px; font-size: 18px; width: 140px; }
  #temp { font-size: 40px; margin-top: 20px; }
</style>
</head>
<body>
<h2>SmartDevices – Licht & Temperatur</h2>

<h3>Lichtsteuerung</h3>
<button class="btn" onclick="fetch('/led?state=on')">Licht AN</button>
<button class="btn" onclick="fetch('/led?state=off')">Licht AUS</button>

<h3>Temperatur</h3>
<div id="temp">--.- °C</div>

<script>
setInterval(() => {
  fetch('/temperature')
    .then(r => r.text())
    .then(t => document.getElementById('temp').innerHTML = t + ' °C');
}, 2000);
</script>

</body>
</html>
)rawliteral";

// ===== ROUTEN =====
void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handleLed() {
  if (server.hasArg("state")) {
    String s = server.arg("state");
    if (s == "on") {
      digitalWrite(LED_PIN, LOW);
      lightState = true;
    } else if (s == "off") {
      digitalWrite(LED_PIN, HIGH);
      lightState = false;
    }
  }
  server.send(200, "text/plain", lightState ? "on" : "off");
}

void handleTemperature() {
  float t = getTemperature();
  server.send(200, "text/plain", String(t, 1));
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN ");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
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

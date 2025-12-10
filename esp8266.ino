#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

// ===== WLAN-DATEN ANPASSEN =====
const char *ssid = "wlan1313";
const char *password = "wlan1313pw";

// ===== LED-PIN =====
// NodeMCU: eingebaute LED = GPIO2 (D4)
const int LED_PIN = 2;

// ===== Webserver =====
ESP8266WebServer server(80);

// ===== Globale Variablen =====
bool lightState = false;

// ===== Temperatur-Funktion (DUMMY) =====
// Wird später durch echten Sensor ersetzt
float getTemperature() { return 24.7; }

// ===== HTML-SEITE =====
const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>SmartDevices Steuerung</title>
<link rel="preconnect" href="https://fonts.googleapis.com">
<link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap" rel="stylesheet">
<style>
  * {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
  }

  body {
    font-family: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
    min-height: 100vh;
    display: flex;
    align-items: center;
    justify-content: center;
    background: linear-gradient(135deg, hsl(230, 40%, 12%), hsl(260, 40%, 18%), hsl(280, 40%, 15%));
    color: hsl(0, 0%, 95%);
    padding: 20px;
    position: relative;
    overflow: hidden;
  }

  body::before {
    content: '';
    position: absolute;
    width: 500px;
    height: 500px;
    background: radial-gradient(circle, hsla(270, 70%, 50%, 0.3), transparent 70%);
    top: -200px;
    right: -200px;
    animation: float 8s ease-in-out infinite;
  }

  body::after {
    content: '';
    position: absolute;
    width: 400px;
    height: 400px;
    background: radial-gradient(circle, hsla(200, 70%, 50%, 0.25), transparent 70%);
    bottom: -150px;
    left: -150px;
    animation: float 10s ease-in-out infinite reverse;
  }

  @keyframes float {
    0%, 100% { transform: translate(0, 0) scale(1); }
    50% { transform: translate(30px, 30px) scale(1.1); }
  }

  .container {
    max-width: 500px;
    width: 100%;
    position: relative;
    z-index: 1;
  }

  h1 {
    font-size: 2rem;
    font-weight: 700;
    text-align: center;
    margin-bottom: 2rem;
    background: linear-gradient(135deg, hsl(280, 80%, 70%), hsl(200, 80%, 70%));
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
    animation: slideDown 0.6s ease-out;
  }

  @keyframes slideDown {
    from {
      opacity: 0;
      transform: translateY(-20px);
    }
    to {
      opacity: 1;
      transform: translateY(0);
    }
  }

  .card {
    background: rgba(255, 255, 255, 0.05);
    backdrop-filter: blur(20px);
    border-radius: 24px;
    padding: 2rem;
    margin-bottom: 1.5rem;
    border: 1px solid rgba(255, 255, 255, 0.1);
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
    animation: fadeInUp 0.6s ease-out;
    animation-fill-mode: both;
    transition: transform 0.3s ease, box-shadow 0.3s ease;
  }

  .card:hover {
    transform: translateY(-4px);
    box-shadow: 0 12px 40px rgba(0, 0, 0, 0.4);
  }

  .card:nth-child(2) { animation-delay: 0.1s; }
  .card:nth-child(3) { animation-delay: 0.2s; }

  @keyframes fadeInUp {
    from {
      opacity: 0;
      transform: translateY(30px);
    }
    to {
      opacity: 1;
      transform: translateY(0);
    }
  }

  .card-title {
    font-size: 1.1rem;
    font-weight: 600;
    margin-bottom: 1.5rem;
    color: hsl(0, 0%, 90%);
    display: flex;
    align-items: center;
    gap: 0.5rem;
  }

  .icon {
    font-size: 1.4rem;
  }

  .controls {
    display: flex;
    gap: 1rem;
    justify-content: center;
  }

  .btn {
    flex: 1;
    padding: 1rem 1.5rem;
    font-size: 1rem;
    font-weight: 600;
    border: none;
    border-radius: 16px;
    cursor: pointer;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    font-family: 'Inter', sans-serif;
    position: relative;
    overflow: hidden;
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.2);
  }

  .btn::before {
    content: '';
    position: absolute;
    top: 50%;
    left: 50%;
    width: 0;
    height: 0;
    border-radius: 50%;
    background: rgba(255, 255, 255, 0.3);
    transform: translate(-50%, -50%);
    transition: width 0.6s, height 0.6s;
  }

  .btn:active::before {
    width: 300px;
    height: 300px;
  }

  .btn-on {
    background: linear-gradient(135deg, hsl(140, 70%, 50%), hsl(160, 70%, 45%));
    color: white;
  }

  .btn-on:hover {
    transform: translateY(-2px) scale(1.02);
    box-shadow: 0 8px 20px hsla(140, 70%, 50%, 0.4);
  }

  .btn-on:active {
    transform: translateY(0) scale(0.98);
  }

  .btn-off {
    background: linear-gradient(135deg, hsl(0, 65%, 55%), hsl(10, 70%, 50%));
    color: white;
  }

  .btn-off:hover {
    transform: translateY(-2px) scale(1.02);
    box-shadow: 0 8px 20px hsla(0, 65%, 55%, 0.4);
  }

  .btn-off:active {
    transform: translateY(0) scale(0.98);
  }

  .status-indicator {
    display: inline-flex;
    align-items: center;
    gap: 0.5rem;
    margin-top: 1rem;
    padding: 0.5rem 1rem;
    background: rgba(255, 255, 255, 0.05);
    border-radius: 12px;
    font-size: 0.9rem;
    transition: all 0.3s ease;
  }

  .status-dot {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: hsl(0, 0%, 50%);
    animation: pulse 2s ease-in-out infinite;
  }

  .status-dot.active {
    background: hsl(140, 70%, 50%);
    box-shadow: 0 0 12px hsla(140, 70%, 50%, 0.6);
  }

  @keyframes pulse {
    0%, 100% { opacity: 1; transform: scale(1); }
    50% { opacity: 0.7; transform: scale(1.1); }
  }

  .temp-display {
    font-size: 4rem;
    font-weight: 700;
    text-align: center;
    background: linear-gradient(135deg, hsl(40, 90%, 60%), hsl(10, 85%, 60%));
    -webkit-background-clip: text;
    -webkit-text-fill-color: transparent;
    background-clip: text;
    margin-top: 1rem;
    transition: all 0.5s ease;
    animation: tempPulse 3s ease-in-out infinite;
  }

  @keyframes tempPulse {
    0%, 100% { transform: scale(1); }
    50% { transform: scale(1.02); }
  }

  .temp-label {
    text-align: center;
    font-size: 0.9rem;
    color: hsl(0, 0%, 70%);
    margin-top: 0.5rem;
  }

  @media (max-width: 600px) {
    h1 { font-size: 1.5rem; }
    .card { padding: 1.5rem; }
    .temp-display { font-size: 3rem; }
    .controls { flex-direction: column; }
  }

  .loading {
    opacity: 0.5;
    pointer-events: none;
  }
</style>
</head>
<body>
<div class="container">
  <h1>🏠 SmartDevices Control</h1>

  <div class="card">
    <div class="card-title">
      <span class="icon">💡</span>
      Lichtsteuerung
    </div>
    <div class="controls">
      <button class="btn btn-on" onclick="toggleLight('on')">
        <span>Licht AN</span>
      </button>
      <button class="btn btn-off" onclick="toggleLight('off')">
        <span>Licht AUS</span>
      </button>
    </div>
    <div style="text-align: center;">
      <div class="status-indicator">
        <span class="status-dot" id="statusDot"></span>
        <span id="statusText">Unbekannt</span>
      </div>
    </div>
  </div>

  <div class="card">
    <div class="card-title">
      <span class="icon">🌡️</span>
      Temperatur
    </div>
    <div class="temp-display" id="temp">--.-°C</div>
    <div class="temp-label">Aktuelle Raumtemperatur</div>
  </div>
</div>

<script>
let isLightOn = false;

function updateStatus(state) {
  const dot = document.getElementById('statusDot');
  const text = document.getElementById('statusText');
  
  if (state === 'on') {
    isLightOn = true;
    dot.classList.add('active');
    text.textContent = 'Licht ist AN';
  } else {
    isLightOn = false;
    dot.classList.remove('active');
    text.textContent = 'Licht ist AUS';
  }
}

async function toggleLight(state) {
  try {
    const response = await fetch('/led?state=' + state);
    if (response.ok) {
      updateStatus(state);
    }
  } catch (error) {
    console.error('Fehler beim Schalten:', error);
  }
}

async function updateTemperature() {
  try {
    const response = await fetch('/temperature');
    const temp = await response.text();
    const tempElement = document.getElementById('temp');
    tempElement.style.transform = 'scale(1.05)';
    setTimeout(() => {
      tempElement.innerHTML = temp + '°C';
      tempElement.style.transform = 'scale(1)';
    }, 150);
  } catch (error) {
    console.error('Fehler beim Abrufen der Temperatur:', error);
  }
}

// Update temperature every 2 seconds
setInterval(updateTemperature, 2000);

// Initial temperature fetch
updateTemperature();
</script>

</body>
</html>
)rawliteral";

// ===== ROUTEN =====
void handleRoot() { server.send(200, "text/html", MAIN_page); }

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

void loop() { server.handleClient(); }

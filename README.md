# ESP8266 SmartDevices Steuerungssystem

Ein modernes webbasiertes Steuerungssystem für ESP8266-Mikrocontroller zur Steuerung von Lichtern und Überwachung der Temperatur mit einer schönen, animierten Benutzeroberfläche.

## Überblick

Dieses Projekt implementiert ein WiFi-fähiges Smart-Home-Steuerungssystem mit einem ESP8266-Mikrocontroller. Es erstellt einen Webserver, der eine moderne, responsive Weboberfläche zur Steuerung angeschlossener Geräte und Überwachung von Sensoren bereitstellt.

## Funktionen

- **LED-Steuerung**: Schalten Sie Lichter über die Weboberfläche per Fernsteuerung ein/aus
- **Temperaturüberwachung**: Echtzeit-Temperaturanzeige (bereit für DS18B20-Sensorintegration)
- **Moderne Web-UI**: Schöne dunkle Benutzeroberfläche mit Animationen
  - Glassmorphismus-Design
  - Flüssige Hover- und Klick-Animationen
  - Responsives Layout für Mobilgeräte und Desktop
  - Statusanzeigen mit pulsierenden Effekten
- **WiFi-Konnektivität**: Von jedem Gerät in Ihrem Netzwerk zugänglich

## Hardware-Anforderungen

- **ESP8266 NodeMCU** oder ähnliches ESP8266-Board
- **Eingebaute LED** (GPIO2/D4 auf NodeMCU)
- **(Optional) DS18B20-Temperatursensor** - Verbinden Sie mit GPIO5 (D1) für echte Temperaturmessungen

## Software-Anforderungen

### Arduino IDE Bibliotheken:

- `ESP8266WiFi` - WiFi-Konnektivität (im ESP8266-Board-Paket enthalten)
- `ESP8266WebServer` - Webserver-Funktionalität (im ESP8266-Board-Paket enthalten)

### Optional (für echten Temperatursensor):

- `OneWire` - Für DS18B20-Kommunikation
- `DallasTemperature` - Für DS18B20-Temperatursensor

## Konfiguration

### WiFi-Einstellungen

Bearbeiten Sie diese Zeilen in `esp8266.ino`:

```cpp
const char* ssid = "test";        // Ihr WiFi-Netzwerkname
const char* password = "testpw";  // Ihr WiFi-Passwort
```

### LED-Pin

```cpp
const int LED_PIN = 2;  // GPIO2 (D4 auf NodeMCU)
```

## Dateistruktur

### `esp8266.ino`

Haupt-Arduino-Sketch mit:

- WiFi-Verbindungseinrichtung
- Webserver-Konfiguration
- HTML/CSS/JavaScript-Oberfläche (eingebettet in PROGMEM)
- API-Endpunkte:
  - `/` - Haupt-Weboberfläche
  - `/led?state=on|off` - LED-Steuerungs-Endpunkt
  - `/temperature` - Temperaturauslesungs-Endpunkt

### `test.html`

Eigenständige HTML-Datei zum Testen und zur Vorschau. Enthält dieselbe Oberfläche wie in der `.ino`-Datei eingebettet.

## Installation & Verwendung

1. **ESP8266-Board-Unterstützung installieren**:

   - Öffnen Sie Arduino IDE
   - Gehen Sie zu Datei → Voreinstellungen
   - Fügen Sie unter "Zusätzliche Boardverwalter-URLs" hinzu:
     `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Gehen Sie zu Werkzeuge → Board → Boardverwalter
   - Suchen Sie nach "esp8266" und installieren Sie es

2. **WiFi konfigurieren**:

   - Bearbeiten Sie `ssid` und `password` im Sketch

3. **Hochladen**:

   - Wählen Sie Board: Werkzeuge → Board → ESP8266 Boards → NodeMCU 1.0
   - Wählen Sie Port: Werkzeuge → Port → (Ihr ESP8266-Port)
   - Klicken Sie auf Hochladen

4. **IP-Adresse finden**:

   - Öffnen Sie den Seriellen Monitor (115200 Baud)
   - Warten Sie auf die Meldung "Connected!"
   - Notieren Sie die angezeigte IP-Adresse (z.B. `192.168.1.123`)

5. **Zugriff auf die Oberfläche**:
   - Öffnen Sie einen Webbrowser
   - Navigieren Sie zur IP-Adresse, die im Seriellen Monitor angezeigt wird
   - Genießen Sie die moderne Steuerungsoberfläche!

## Web-Oberflächen-Funktionen

Die eingebettete Weboberfläche umfasst:

- **Verlaufshintergrund** mit animierten schwebenden Kugeln
- **Glassmorphe Karten** mit Unschärfeeffekten
- **Interaktive Schaltflächen** mit Wellen-Animationen
- **Statusanzeige** zeigt Lichtzustand mit pulsierendem Punkt
- **Temperaturanzeige** mit Verlaufstext und subtiler Puls-Animation
- **Responsives Design** passt sich allen Bildschirmgrößen an

## API-Endpunkte

### `GET /`

Gibt die Haupt-HTML-Oberfläche zurück

### `GET /led?state=on`

Schaltet die LED ein

- Gibt zurück: `"on"` (text/plain)

### `GET /led?state=off`

Schaltet die LED aus

- Gibt zurück: `"off"` (text/plain)

### `GET /temperature`

Gibt die aktuelle Temperaturmessung zurück

- Gibt zurück: `"24.7"` (text/plain, Dummy-Wert)
- Bereit für DS18B20-Sensorintegration

## Hinzufügen eines echten Temperatursensors (DS18B20)

Um einen echten DS18B20-Temperatursensor zu verwenden:

1. **Bibliotheken installieren**:

   - OneWire
   - DallasTemperature

2. **Hardware anschließen**:

   - DS18B20 Data → GPIO5 (D1)
   - DS18B20 VCC → 3.3V
   - DS18B20 GND → GND
   - 4,7kΩ Pull-up-Widerstand zwischen Data und VCC hinzufügen

3. **Code aktualisieren**:
   - Kommentieren Sie den DS18B20-Code im Sketch aus
   - Ersetzen Sie die Dummy-Funktion `getTemperature()` durch tatsächliches Sensorauslesen

## Fehlerbehebung

**Kann keine Verbindung zu WiFi herstellen:**

- Überprüfen Sie SSID und Passwort
- Stellen Sie sicher, dass 2,4 GHz WiFi verwendet wird (ESP8266 unterstützt kein 5 GHz)
- Überprüfen Sie den Seriellen Monitor auf Verbindungsmeldungen

**Kann nicht auf die Weboberfläche zugreifen:**

- Überprüfen Sie die IP-Adresse im Seriellen Monitor
- Stellen Sie sicher, dass sich das Gerät im selben Netzwerk wie der ESP8266 befindet
- Versuchen Sie, die IP-Adresse zu pingen

**LED funktioniert nicht:**

- Hinweis: NodeMCU-LED ist aktiv LOW (leuchtet, wenn Pin LOW ist)
- Überprüfen Sie, ob LED_PIN zu Ihrem Board passt

## Technische Details

- **Mikrocontroller**: ESP8266
- **Betriebsspannung**: 3,3V
- **WiFi**: 802.11 b/g/n (2,4 GHz)
- **Webserver**: ESP8266WebServer-Bibliothek
- **Temperaturaktualisierungsintervall**: 2 Sekunden
- **Serielle Baudrate**: 115200

## Pädagogische Hinweise

Dieses Projekt demonstriert:

- IoT-Geräteprogrammierung
- Webserver-Implementierung auf Mikrocontrollern
- RESTful-API-Design
- Entwicklung moderner Weboberflächen
- Prinzipien des responsiven Designs
- CSS-Animationen und Übergänge
- Asynchrones JavaScript (async/await)

## Lizenz

Bildungsprojekt für den Berufsschul-ITT-Kurs.

## Autoren

[Mike Fox](https://github.com/MikeLeonFox)
[Dimitrios Kotsis](https://github.com/hamsterbacke21)

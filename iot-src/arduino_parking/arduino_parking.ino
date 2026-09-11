/*
 * ============================================================
 *  Arduino UNO R4 Wifi — Logique Centrale Parking
 *  VERSION STABLE — Sans communication Symfony
 *
 *  Réseau : Se connecte au réseau "ParkingCam" (ESP32 existante)
 *  IP     : 192.168.4.2 (fixe)
 *
 *  Routes disponibles :
 *    POST /open  → ouvre la barrière entrée (appelé par Python)
 *
 *  Dépendances :
 *    → ESP32 nouvelle  : 192.168.4.3 (actionneurs)
 *    → ESP32 existante : 192.168.4.1 (réseau ParkingCam)
 *
 *  Branchements HC-SR04 :
 *    Place 1    → TRIG:D2  ECHO:D3
 *    Place 2    → TRIG:D4  ECHO:D5
 *    Place 3    → TRIG:D6  ECHO:D7
 *    Place 4    → TRIG:D8  ECHO:D9
 *    Barrière   → TRIG:D10 ECHO:D11
 *
 *  LCD I2C :
 *    SDA → A4 | SCL → A5
 * ============================================================
 */

#include <Arduino.h>
#include <WiFiS3.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ─── CONFIG RÉSEAU ───────────────────────────────────────────
#define AP_SSID      "ParkingCam"
#define AP_PASSWORD  "12345678"

IPAddress local_IP(192, 168, 4, 2);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

const char* ESP32_IP   = "192.168.4.3";
const int   ESP32_PORT = 80;

// ─── PINS HC-SR04 ────────────────────────────────────────────
#define TRIG_P1  2
#define ECHO_P1  3
#define TRIG_P2  4
#define ECHO_P2  5
#define TRIG_P3  6
#define ECHO_P3  7
#define TRIG_P4  8
#define ECHO_P4  9
#define TRIG_BAR 10
#define ECHO_BAR 11

// ─── SEUILS DÉTECTION ────────────────────────────────────────
#define SEUIL_PLACE    3
#define SEUIL_BARRIERE 3

// ─── TIMING ──────────────────────────────────────────────────
#define DELAI_SCAN        50
#define DUREE_BARRIERE   3000
#define BUZZER_DURATION   500
#define COOLDOWN_BARRIERE 5000

// ─── LCD ─────────────────────────────────────────────────────
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ─── ÉTAT DES PLACES ─────────────────────────────────────────
bool placeOccupee[4]          = {false, false, false, false};
bool barriereSortieOuverte    = false;
unsigned long dernierOuvertureSortie = 0;
unsigned long dernierScanPlace[4]    = {0, 0, 0, 0};
unsigned long dernierScanBarriere    = 0;

// ─── SERVEUR HTTP ────────────────────────────────────────────
WiFiServer httpServer(80);

// ─────────────────────────────────────────────────────────────

// ─── MESURE DISTANCE HC-SR04 ─────────────────────────────────
float mesurerDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return 999.0;
  return (duration * 0.0343) / 2.0;
}

// ─── ENVOI REQUÊTE HTTP vers ESP32 nouvelle ──────────────────
bool envoyerRequete(String route, String body) {
  WiFiClient client;

  if (!client.connect(ESP32_IP, ESP32_PORT)) {
    client.stop();
    return false;
  }

  client.println("POST " + route + " HTTP/1.1");
  client.println("Host: " + String(ESP32_IP));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(body.length()));
  client.println("Connection: close");
  client.println();
  client.println(body);

  unsigned long timeout = millis() + 500;
  while (client.available() == 0 && millis() < timeout);

  String statusLine = "";
  if (client.available()) {
    statusLine = client.readStringUntil('\n');
  }

  while (client.available()) client.read();
  client.stop();

  bool ok = statusLine.indexOf("200") >= 0;
  Serial.print("[HTTP] "); Serial.print(route);
  Serial.print(" -> "); Serial.println(ok ? "200 OK" : "ERREUR");
  return ok;
}

// ─── COMMANDES ESP32 NOUVELLE ────────────────────────────────
void ouvrirBarriere(String servo) {
  envoyerRequete("/servo-open", "{\"servo\":\"" + servo + "\"}");
  int buzzerId = (servo == "entree") ? 1 : 2;
  envoyerRequete("/buzzer",
    "{\"id\":" + String(buzzerId) + ",\"duration\":" + String(BUZZER_DURATION) + "}");
  Serial.print("[Barriere] "); Serial.print(servo); Serial.println(" -> ouverte");
}

void fermerBarriere(String servo) {
  envoyerRequete("/servo-close", "{\"servo\":\"" + servo + "\"}");
  Serial.print("[Barriere] "); Serial.print(servo); Serial.println(" -> fermee");
}

void setLedPlace(int place, String color) {
  envoyerRequete("/led",
    "{\"place\":" + String(place) + ",\"color\":\"" + color + "\"}");
  Serial.print("[LED] Place "); Serial.print(place);
  Serial.print(" -> "); Serial.println(color);
}

// ─── MISE À JOUR LCD ─────────────────────────────────────────
void mettreAJourLCD() {
  int placesLibres = 0;
  for (int i = 0; i < 4; i++) {
    if (!placeOccupee[i]) placesLibres++;
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Parking DaVinci");
  lcd.setCursor(0, 1);

  if (placesLibres == 0) {
    lcd.print("COMPLET         ");
  } else {
    lcd.print("Libres: ");
    lcd.print(placesLibres);
    lcd.print("/4      ");
  }
}

// ─── GESTION REQUÊTES HTTP ENTRANTES ─────────────────────────
void gererRequetesEntrantes() {
  WiFiClient client = httpServer.available();
  if (!client) return;

  unsigned long timeout = millis() + 500;
  String requestLine = "";

  while (client.connected() && millis() < timeout) {
    if (client.available()) {
      char c = client.read();
      requestLine += c;
      if (requestLine.endsWith("\r\n\r\n")) break;
    }
  }

  Serial.println("[HTTP] Requete recue");

  if (requestLine.startsWith("POST /open")) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println("{\"status\":\"ok\",\"duration_ms\":" + String(DUREE_BARRIERE) + "}");
    client.stop();

    ouvrirBarriere("entree");
    delay(DUREE_BARRIERE);
    fermerBarriere("entree");

  } else {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
    client.println("{\"error\":\"Route non trouvee\"}");
    client.stop();
  }
}

// ─── SCAN DES PLACES ─────────────────────────────────────────
void scannerPlaces() {
  int trigPins[] = {TRIG_P1, TRIG_P2, TRIG_P3, TRIG_P4};
  int echoPins[] = {ECHO_P1, ECHO_P2, ECHO_P3, ECHO_P4};

  for (int i = 0; i < 4; i++) {
    if (millis() - dernierScanPlace[i] < DELAI_SCAN) continue;
    dernierScanPlace[i] = millis();

    float dist   = mesurerDistance(trigPins[i], echoPins[i]);
    bool occupee = (dist < SEUIL_PLACE);

    Serial.print("[HC-SR04] Place "); Serial.print(i + 1);
    Serial.print(" : "); Serial.print(dist);
    Serial.print(" cm -> "); Serial.println(occupee ? "OCCUPEE" : "libre");

    if (occupee != placeOccupee[i]) {
      placeOccupee[i] = occupee;
      setLedPlace(i + 1, occupee ? "red" : "green");
      mettreAJourLCD();
    }
  }
}

// ─── SCAN BARRIÈRE SORTIE ────────────────────────────────────
void scannerBarriereSortie() {
  if (millis() - dernierOuvertureSortie < COOLDOWN_BARRIERE) return;
  if (millis() - dernierScanBarriere < DELAI_SCAN) return;
  dernierScanBarriere = millis();

  float dist = mesurerDistance(TRIG_BAR, ECHO_BAR);
  Serial.print("[HC-SR04] Barriere sortie : ");
  Serial.print(dist); Serial.println(" cm");

  if (dist < SEUIL_BARRIERE && !barriereSortieOuverte) {
    barriereSortieOuverte  = true;
    dernierOuvertureSortie = millis();

    ouvrirBarriere("sortie");
    delay(DUREE_BARRIERE);
    fermerBarriere("sortie");

    barriereSortieOuverte = false;
  }
}

// ─── SETUP ───────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Arduino UNO R4 Wifi — Parking Boot ===");

  int trigPins[] = {TRIG_P1, TRIG_P2, TRIG_P3, TRIG_P4, TRIG_BAR};
  int echoPins[] = {ECHO_P1, ECHO_P2, ECHO_P3, ECHO_P4, ECHO_BAR};
  for (int i = 0; i < 5; i++) {
    pinMode(trigPins[i], OUTPUT);
    pinMode(echoPins[i], INPUT);
    digitalWrite(trigPins[i], LOW);
  }
  Serial.println("[HC-SR04] Pins initialises");

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Parking DaVinci");
  lcd.setCursor(0, 1);
  lcd.print("Demarrage...");
  Serial.println("[LCD] Initialise");

  WiFi.config(local_IP, gateway, subnet);
  WiFi.begin(AP_SSID, AP_PASSWORD);

  Serial.print("[WiFi] Connexion a ParkingCam");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Connecte");
    Serial.print("[WiFi] IP : "); Serial.println(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print("WiFi OK         ");
  } else {
    Serial.println("\n[WiFi] Connexion echouee");
    lcd.setCursor(0, 1);
    lcd.print("WiFi ERREUR     ");
  }

  httpServer.begin();
  Serial.println("[HTTP] Serveur demarre -> POST /open");

  delay(1000);
  mettreAJourLCD();
  Serial.println("==========================================");
}

// ─── LOOP ────────────────────────────────────────────────────
void loop() {
  gererRequetesEntrantes();
  scannerPlaces();
  scannerBarriereSortie();
}

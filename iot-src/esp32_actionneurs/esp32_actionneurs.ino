/*
 * ============================================================
 *  ESP32 Nouvelle — Actionneurs Parking
 *  VERSION STABLE
 *
 *  Rôle : Serveur HTTP qui reçoit les commandes de l'Arduino
 *         et contrôle les servos, LEDs RGB et buzzers
 *
 *  Réseau : Se connecte au réseau "ParkingCam" (ESP32 existante)
 *  IP     : 192.168.4.3 (fixe)
 *
 *  Routes disponibles :
 *    POST /servo-open   → {"servo": "entree"} ou {"servo": "sortie"}
 *    POST /servo-close  → {"servo": "entree"} ou {"servo": "sortie"}
 *    POST /led          → {"place": 1, "color": "red"|"green"|"yellow"|"off"}
 *    POST /buzzer       → {"id": 1, "duration": 500}
 *    GET  /status       → état du système
 *    GET  /test         → vérification serveur
 *
 *  Branchements :
 *    Servo entrée       → GPIO 12
 *    Servo sortie       → GPIO 13
 *    LED Place 1 R/G    → GPIO 18 / 19
 *    LED Place 2 R/G    → GPIO 22 / 23
 *    LED Place 3 R/G    → GPIO 14 / 27
 *    LED Place 4 R/G    → GPIO 25 / 33
 *    Buzzer 1           → GPIO 15
 *    Buzzer 2           → GPIO 4
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include <ArduinoJson.h>

// ─── CONFIG RÉSEAU ───────────────────────────────────────────
#define AP_SSID      "ParkingCam"
#define AP_PASSWORD  "12345678"
IPAddress local_IP(192, 168, 4, 3);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// ─── PINS SERVOS ─────────────────────────────────────────────
#define SERVO_ENTREE_PIN  12
#define SERVO_SORTIE_PIN  13

// ─── ANGLES SERVOS ───────────────────────────────────────────
#define SERVO_OUVERT_ENTREE   90  // verticale = barrière levée
#define SERVO_FERME_ENTREE   180  // horizontale = barrière fermée
#define SERVO_OUVERT_SORTIE  180  // barrière sortie levée vers le haut
#define SERVO_FERME_SORTIE    90  // barrière sortie fermée horizontale

// ─── PINS LEDs RGB ───────────────────────────────────────────
#define LED1_R  18
#define LED1_G  19
#define LED2_R  22
#define LED2_G  23
#define LED3_R  14
#define LED3_G  27
#define LED4_R  25
#define LED4_G  33

// ─── PINS BUZZERS ────────────────────────────────────────────
#define BUZZER_1  15
#define BUZZER_2   4

// ─── DURÉE BUZZER (ms) ───────────────────────────────────────
#define BUZZER_DURATION  500

// ─────────────────────────────────────────────────────────────

Servo servoEntree;
Servo servoSortie;
WebServer server(80);

String ledState[4] = {"green", "green", "green", "green"};

// ─── UTILITAIRES LEDs ────────────────────────────────────────
int getLedRPin(int place) {
  switch (place) {
    case 1: return LED1_R;
    case 2: return LED2_R;
    case 3: return LED3_R;
    case 4: return LED4_R;
    default: return -1;
  }
}

int getLedGPin(int place) {
  switch (place) {
    case 1: return LED1_G;
    case 2: return LED2_G;
    case 3: return LED3_G;
    case 4: return LED4_G;
    default: return -1;
  }
}

void setLed(int place, String color) {
  int pinR = getLedRPin(place);
  int pinG = getLedGPin(place);
  if (pinR == -1 || pinG == -1) return;

  if (color == "red") {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, LOW);
  } else if (color == "green") {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, HIGH);
  } else if (color == "yellow") {
    digitalWrite(pinR, HIGH);
    digitalWrite(pinG, HIGH);
  } else if (color == "off") {
    digitalWrite(pinR, LOW);
    digitalWrite(pinG, LOW);
  }
  ledState[place - 1] = color;
}

// ─── HANDLERS HTTP ───────────────────────────────────────────

// POST /servo-open
void handleServoOpen() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
    return;
  }

  String servo = doc["servo"] | "";
  if (servo == "entree") {
    servoEntree.write(SERVO_OUVERT_ENTREE);
    Serial.println("[Servo] Barriere ENTREE -> ouverte");
    server.send(200, "application/json", "{\"status\":\"ok\",\"servo\":\"entree\",\"position\":\"ouvert\"}");
  } else if (servo == "sortie") {
    servoSortie.write(SERVO_OUVERT_SORTIE);
    Serial.println("[Servo] Barriere SORTIE -> ouverte");
    server.send(200, "application/json", "{\"status\":\"ok\",\"servo\":\"sortie\",\"position\":\"ouvert\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Valeur servo invalide. Utiliser: entree ou sortie\"}");
  }
}

// POST /servo-close
void handleServoClose() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
    return;
  }

  String servo = doc["servo"] | "";
  if (servo == "entree") {
    servoEntree.write(SERVO_FERME_ENTREE);
    Serial.println("[Servo] Barriere ENTREE -> fermee");
    server.send(200, "application/json", "{\"status\":\"ok\",\"servo\":\"entree\",\"position\":\"ferme\"}");
  } else if (servo == "sortie") {
    servoSortie.write(SERVO_FERME_SORTIE);
    Serial.println("[Servo] Barriere SORTIE -> fermee");
    server.send(200, "application/json", "{\"status\":\"ok\",\"servo\":\"sortie\",\"position\":\"ferme\"}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Valeur servo invalide. Utiliser: entree ou sortie\"}");
  }
}

// POST /led
void handleLed() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
    return;
  }

  int    place = doc["place"] | 0;
  String color = doc["color"] | "";

  if (place < 1 || place > 4) {
    server.send(400, "application/json", "{\"error\":\"place doit etre entre 1 et 4\"}");
    return;
  }
  if (color != "red" && color != "green" && color != "yellow" && color != "off") {
    server.send(400, "application/json", "{\"error\":\"color doit etre red, green, yellow ou off\"}");
    return;
  }

  setLed(place, color);
  Serial.print("[LED] Place "); Serial.print(place);
  Serial.print(" -> "); Serial.println(color);

  String response = "{\"status\":\"ok\",\"place\":" + String(place) + ",\"color\":\"" + color + "\"}";
  server.send(200, "application/json", response);
}

// POST /buzzer
void handleBuzzer() {
  if (server.method() != HTTP_POST) {
    server.send(405, "application/json", "{\"error\":\"Method not allowed\"}");
    return;
  }

  StaticJsonDocument<128> doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    server.send(400, "application/json", "{\"error\":\"JSON invalide\"}");
    return;
  }

  int id       = doc["id"]       | 1;
  int duration = doc["duration"] | BUZZER_DURATION;

  if (id != 1 && id != 2) {
    server.send(400, "application/json", "{\"error\":\"id doit etre 1 ou 2\"}");
    return;
  }

  int pin = (id == 1) ? BUZZER_1 : BUZZER_2;

  server.send(200, "application/json",
    "{\"status\":\"ok\",\"buzzer\":" + String(id) + ",\"duration\":" + String(duration) + "}");

  tone(pin, 1000, duration);
  delay(duration + 50);
  noTone(pin);

  Serial.print("[Buzzer] Buzzer "); Serial.print(id);
  Serial.print(" -> "); Serial.print(duration); Serial.println("ms");
}

// GET /status
void handleStatus() {
  String json = "{";
  json += "\"servos\":{";
  json += "\"entree\":" + String(servoEntree.read() == SERVO_OUVERT_ENTREE ? "\"ouvert\"" : "\"ferme\"") + ",";
  json += "\"sortie\":" + String(servoSortie.read() == SERVO_OUVERT_SORTIE ? "\"ouvert\"" : "\"ferme\"");
  json += "},";
  json += "\"leds\":{";
  json += "\"place1\":\"" + ledState[0] + "\",";
  json += "\"place2\":\"" + ledState[1] + "\",";
  json += "\"place3\":\"" + ledState[2] + "\",";
  json += "\"place4\":\"" + ledState[3] + "\"";
  json += "}";
  json += "}";
  server.send(200, "application/json", json);
}

// GET /test
void handleTest() {
  server.send(200, "text/plain", "ESP32 Actionneurs OK");
}

// 404
void handleNotFound() {
  server.send(404, "application/json", "{\"error\":\"Route non trouvee\"}");
}

// ─── SETUP ───────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32 Actionneurs Boot ===");

  // ── LEDs RGB ──
  int ledPins[] = {LED1_R, LED1_G, LED2_R, LED2_G,
                   LED3_R, LED3_G, LED4_R, LED4_G};
  for (int pin : ledPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  for (int i = 1; i <= 4; i++) setLed(i, "green");
  Serial.println("[LED] Toutes les places -> vert");

  // ── Buzzers ──
  pinMode(BUZZER_1, OUTPUT);
  pinMode(BUZZER_2, OUTPUT);
  digitalWrite(BUZZER_1, LOW);
  digitalWrite(BUZZER_2, LOW);
  Serial.println("[Buzzer] Initialises");

  // ── Servos ──
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  servoEntree.setPeriodHertz(50);
  servoSortie.setPeriodHertz(50);
  servoEntree.attach(SERVO_ENTREE_PIN, 500, 2400);
  servoSortie.attach(SERVO_SORTIE_PIN, 500, 2400);
  servoEntree.write(SERVO_FERME_ENTREE);
  servoSortie.write(SERVO_FERME_SORTIE);
  Serial.println("[Servo] Barrieres -> fermees");

  // ── WiFi ──
  WiFi.mode(WIFI_STA);
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
    Serial.print("[WiFi] IP : ");
    Serial.println(WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n[WiFi] Connexion echouee — verifier que ParkingCam est actif");
  }

  // ── Routes HTTP ──
  server.on("/servo-open",  HTTP_POST, handleServoOpen);
  server.on("/servo-close", HTTP_POST, handleServoClose);
  server.on("/led",         HTTP_POST, handleLed);
  server.on("/buzzer",      HTTP_POST, handleBuzzer);
  server.on("/status",      HTTP_GET,  handleStatus);
  server.on("/test",        HTTP_GET,  handleTest);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("[HTTP] Serveur demarre");
  Serial.println("  POST /servo-open   {\"servo\":\"entree\"|\"sortie\"}");
  Serial.println("  POST /servo-close  {\"servo\":\"entree\"|\"sortie\"}");
  Serial.println("  POST /led          {\"place\":1-4,\"color\":\"red\"|\"green\"|\"yellow\"|\"off\"}");
  Serial.println("  POST /buzzer       {\"id\":1-2,\"duration\":500}");
  Serial.println("  GET  /status");
  Serial.println("  GET  /test");
  Serial.println("==============================");
}

// ─── LOOP ────────────────────────────────────────────────────
void loop() {
  server.handleClient();
}

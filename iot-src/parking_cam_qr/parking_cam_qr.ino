/*
 * ============================================================
 *  Parking Camera — ESP32 + ArduCAM OV5642 5MP
 *  Mode Access Point : l'ESP32 crée son propre réseau WiFi
 *
 *  Branchage :
 *    CS   → GPIO 5   | SCK  → GPIO 18
 *    MOSI → GPIO 23  | MISO → GPIO 19
 *    SDA  → GPIO 21  | SCL  → GPIO 22
 *    VCC  → 3.3V     | GND  → GND
 *
 *  Connexion :
 *    1. Rejoindre le WiFi "ParkingCam" (mdp: 12345678)
 *    2. Ouvrir http://192.168.4.1
 * ============================================================
 */

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduCAM.h>
#include "memorysaver.h"

// ─── CONFIG ACCESS POINT ─────────────────────────────────────
#define AP_SSID      "ParkingCam"
#define AP_PASSWORD  "12345678"

// ─── CONFIG CAMÉRA ───────────────────────────────────────────
#define CAM_CS_PIN      5
#define CHUNK_SIZE      4096
#define CAM_RESOLUTION  OV5642_320x240
#define CAPTURE_DELAY_MS 500
#define BUTTON_PIN 32
// ─────────────────────────────────────────────────────────────

ArduCAM  cam(OV5642, CAM_CS_PIN);
WebServer server(80);

static uint8_t chunkBuf[CHUNK_SIZE];

// ─── PAGE HTML ───────────────────────────────────────────────
const char HTML_PAGE[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Parking Camera</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
    :root {
      --green: #00ff88; --dim: #00aa55;
      --bg: #020d06; --panel: #071a0e; --border: #0a3020;
    }
    body {
      font-family: 'Share Tech Mono', monospace;
      background: var(--bg); color: var(--green);
      min-height: 100vh;
      display: flex; flex-direction: column;
      align-items: center; justify-content: center;
      padding: 24px;
    }
    body::before {
      content: ''; position: fixed; inset: 0;
      background: repeating-linear-gradient(
        0deg, transparent, transparent 3px,
        rgba(0,255,136,.03) 3px, rgba(0,255,136,.03) 4px
      );
      pointer-events: none; z-index: 10;
    }
    header {
      width: 100%; max-width: 800px;
      display: flex; justify-content: space-between; align-items: center;
      border-bottom: 1px solid var(--border);
      padding-bottom: 12px; margin-bottom: 20px;
    }
    .logo { font-size: 1.1rem; letter-spacing: .3em; text-transform: uppercase; color: var(--green); text-shadow: 0 0 8px var(--green); }
    .badge { font-size: .7rem; letter-spacing: .15em; color: var(--dim); }
    .rec-dot { display: inline-block; width: 8px; height: 8px; background: #ff3344; border-radius: 50%; margin-right: 6px; animation: blink 1s step-end infinite; box-shadow: 0 0 6px #ff3344; }
    @keyframes blink { 50% { opacity: 0; } }
    .cam-frame { position: relative; width: 100%; max-width: 800px; background: var(--panel); border: 1px solid var(--border); border-radius: 4px; overflow: hidden; }
    .cam-frame::before, .cam-frame::after { content: ''; position: absolute; width: 18px; height: 18px; border-color: var(--green); border-style: solid; z-index: 5; opacity: .7; }
    .cam-frame::before { top: 8px; left: 8px; border-width: 2px 0 0 2px; }
    .cam-frame::after  { bottom: 8px; right: 8px; border-width: 0 2px 2px 0; }
    #camImg { width: 100%; display: block; aspect-ratio: 4/3; object-fit: cover; filter: contrast(1.05) saturate(.9); background: #000; }
    .overlay-text { position: absolute; bottom: 14px; left: 14px; font-size: .65rem; color: var(--dim); letter-spacing: .12em; }
    .fps-counter { position: absolute; bottom: 14px; right: 14px; font-size: .65rem; color: var(--dim); letter-spacing: .12em; }
    footer { margin-top: 16px; font-size: .65rem; color: var(--border); letter-spacing: .2em; }
  </style>
</head>
<body>
  <header>
    <span class="logo">&#9632; Parking Monitor</span>
    <span class="badge"><span class="rec-dot"></span>LIVE · OV5642</span>
  </header>
  <div class="cam-frame">
    <img id="camImg" src="" alt="">
    <div class="overlay-text" id="ts">--:--:--</div>
    <div class="fps-counter" id="fps">-- fps</div>
  </div>
  <footer>ESP32 AP · ArduCAM 5MP · JPEG</footer>
  <script>
    const img = document.getElementById('camImg');
    const tsEl = document.getElementById('ts');
    const fpsEl = document.getElementById('fps');
    let frameCount = 0;
    function nextFrame() {
      const next = new Image();
      next.onload = () => { img.src = next.src; frameCount++; setTimeout(nextFrame, 0); };
      next.onerror = () => { setTimeout(nextFrame, 800); };
      next.src = '/capture?t=' + Date.now();
    }
    setInterval(() => { fpsEl.textContent = frameCount + ' fps'; frameCount = 0; }, 1000);
    function tick() { tsEl.textContent = new Date().toLocaleTimeString('fr-FR', { hour12: false }); }
    setInterval(tick, 1000); tick();
    nextFrame();
  </script>
</body>
</html>
)rawhtml";

// ─── CAPTURE UNE FRAME ───────────────────────────────────────
bool captureFrame() {
  cam.flush_fifo();
  cam.clear_fifo_flag();
  cam.start_capture();
  delay(CAPTURE_DELAY_MS);

  uint32_t len = cam.read_fifo_length();
  Serial.print("[CAM] FIFO length : ");
  Serial.println(len);

  if (len == 0 || len >= MAX_FIFO_SIZE) {
    Serial.println("[CAM] Taille FIFO invalide !");
    return false;
  }
  return true;
}

// ─── HANDLER : / ─────────────────────────────────────────────
void handleRoot() {
  server.sendHeader("Cache-Control", "no-cache");
  server.send_P(200, "text/html", HTML_PAGE);
}

// ─── HANDLER : /capture ──────────────────────────────────────
void handleCapture() {
  Serial.println("[HTTP] GET /capture");

  if (!captureFrame()) {
    server.send(503, "text/plain", "Capture failed");
    return;
  }

  uint32_t imgLen = cam.read_fifo_length();
  server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server.sendHeader("Pragma", "no-cache");
  server.setContentLength(imgLen);
  server.send(200, "image/jpeg", "");

  WiFiClient client = server.client();
  cam.CS_LOW();
  cam.set_fifo_burst();

  uint32_t remaining = imgLen;
  while (remaining > 0 && client.connected()) {
    uint32_t toRead = min((uint32_t)CHUNK_SIZE, remaining);
    for (uint32_t i = 0; i < toRead; i++) {
      chunkBuf[i] = SPI.transfer(0x00);
    }
    client.write(chunkBuf, toRead);
    remaining -= toRead;
  }

  cam.CS_HIGH();
  Serial.println("[HTTP] /capture envoye OK");
}

// ─── HANDLER : /test ─────────────────────────────────────────
void handleTest() {
  server.send(200, "text/plain", "ESP32 OK - serveur operationnel");
}

// ─── HANDLER : 404 ───────────────────────────────────────────
void handleButtonStatus() {
  bool pressed = (digitalRead(BUTTON_PIN) == LOW);
  if (pressed) {
    server.send(200, "application/json", "{\"pressed\":true}");
    Serial.println("[Bouton] Appui detecte");
  } else {
    server.send(200, "application/json", "{\"pressed\":false}");
  }
}
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

// ─── INIT CAMÉRA ─────────────────────────────────────────────
bool initCamera() {
  cam.write_reg(ARDUCHIP_TEST1, 0x55);
  uint8_t temp = cam.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55) {
    Serial.print("[CAM] ERREUR SPI — recu 0x");
    Serial.println(temp, HEX);
    return false;
  }
  Serial.println("[CAM] SPI OK");

  uint8_t vid, pid;
  cam.wrSensorReg16_8(0xff, 0x01);
  cam.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  cam.rdSensorReg16_8(OV5642_CHIPID_LOW,  &pid);
  Serial.print("[CAM] Chip ID VID:0x"); Serial.print(vid, HEX);
  Serial.print(" PID:0x"); Serial.println(pid, HEX);
  if (vid != 0x56 || pid != 0x42) {
    Serial.println("[CAM] ERREUR I2C");
    return false;
  }

  cam.wrSensorReg16_8(0x3008, 0x80); delay(200);
  cam.wrSensorReg16_8(0x3008, 0x00); delay(200);

  cam.set_format(JPEG);
  cam.InitCAM();
  delay(500);

  cam.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK);
  cam.write_reg(ARDUCHIP_FRAMES, 0x00);
  cam.OV5642_set_JPEG_size(CAM_RESOLUTION);
  delay(3000);
  cam.clear_fifo_flag();

  Serial.println("[CAM] Test capture...");
  cam.flush_fifo();
  cam.clear_fifo_flag();
  cam.start_capture();
  delay(CAPTURE_DELAY_MS);
  uint32_t testLen = cam.read_fifo_length();
  Serial.print("[CAM] Test FIFO : "); Serial.println(testLen);
  if (testLen < 100) {
    Serial.println("[CAM] AVERTISSEMENT : FIFO trop petit !");
  } else {
    Serial.println("[CAM] Capture OK !");
  }

  cam.clear_fifo_flag();
  return true;
}

// ─── SETUP ───────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("\n=== Parking Camera Boot ===");

  SPI.begin();
  pinMode(CAM_CS_PIN, OUTPUT);
  digitalWrite(CAM_CS_PIN, HIGH);
  Wire.begin(21, 22);
  Wire.setClock(100000);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("[Bouton] GPIO 32 initialise");

  cam.write_reg(0x07, 0x80); delay(100);
  cam.write_reg(0x07, 0x00); delay(100);

  if (!initCamera()) {
    Serial.println("[FATAL] Camera non initialisee.");
    while (true) { delay(1000); }
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(
    IPAddress(192, 168, 4, 1),
    IPAddress(192, 168, 4, 1),
    IPAddress(255, 255, 255, 0)
  );
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  Serial.println("[WiFi] Access Point demarre !");
  Serial.print("[WiFi] SSID : "); Serial.println(AP_SSID);
  Serial.println("[Web]  http://192.168.4.1");

  server.on("/",        HTTP_GET, handleRoot);
  server.on("/capture", HTTP_GET, handleCapture);
  server.on("/test",    HTTP_GET, handleTest);
  server.on("/button-status", HTTP_GET, handleButtonStatus);
  server.onNotFound(handleNotFound);
  server.begin();

  Serial.println("[Web] Serveur demarre");
  Serial.println("===========================");
}

// ─── LOOP ────────────────────────────────────────────────────
void loop() {
  server.handleClient();  
}

#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>

const char* ssid = "MALATYA";
const char* password = "44044044";

WebServer server(80);

#define PIN 18
#define NUMPIXELS 22

Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

String currentMode = "off";

uint8_t currentR = 255;
uint8_t currentG = 0;
uint8_t currentB = 0;

int rainbowOffset = 0;
bool policeState = false;
int waterIndex = 0;
int meteorIndex = 0;
float pulsePhase = 0.0;
float breathPhase = 0.0;
float waveOffset = 0.0;

unsigned long lastUpdate = 0;

// --------------------------------------------------
// TEMEL YARDIMCI FONKSİYONLAR
// --------------------------------------------------

void clearStrip() {
  strip.clear();
  strip.show();
}

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

uint32_t wheel(byte pos) {
  pos = 255 - pos;
  if (pos < 85) {
    return strip.Color(255 - pos * 3, 0, pos * 3);
  }
  if (pos < 170) {
    pos -= 85;
    return strip.Color(0, pos * 3, 255 - pos * 3);
  }
  pos -= 170;
  return strip.Color(pos * 3, 255 - pos * 3, 0);
}

void resetAnimationStates() {
  rainbowOffset = 0;
  policeState = false;
  waterIndex = 0;
  meteorIndex = 0;
  pulsePhase = 0.0;
  breathPhase = 0.0;
  waveOffset = 0.0;
}

// --------------------------------------------------
// SABİT RENK MODU
// --------------------------------------------------

void effectOff() {
  setAllPixels(0, 0, 0);
}

void effectSolid() {
  setAllPixels(currentR, currentG, currentB);
}

// --------------------------------------------------
// RAINBOW
// --------------------------------------------------

void effectRainbowStep() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, wheel((i * 256 / NUMPIXELS + rainbowOffset) & 255));
  }
  strip.show();

  rainbowOffset++;
  if (rainbowOffset >= 256) rainbowOffset = 0;
}

// --------------------------------------------------
// POLICE
// --------------------------------------------------

void effectPoliceStep() {
  for (int i = 0; i < NUMPIXELS; i++) {
    if (i < NUMPIXELS / 2) {
      if (policeState) strip.setPixelColor(i, strip.Color(255, 0, 0));
      else             strip.setPixelColor(i, strip.Color(0, 0, 255));
    } else {
      if (policeState) strip.setPixelColor(i, strip.Color(0, 0, 255));
      else             strip.setPixelColor(i, strip.Color(255, 0, 0));
    }
  }
  strip.show();
  policeState = !policeState;
}

// --------------------------------------------------
// WATER - SEÇİLEN RENKLE
// --------------------------------------------------

void effectWaterStep() {
  strip.clear();

  for (int i = 0; i < NUMPIXELS; i++) {
    int distance = abs(i - waterIndex);

    if (distance == 0) {
      strip.setPixelColor(i, strip.Color(currentR, currentG, currentB));
    }
    else if (distance == 1) {
      strip.setPixelColor(i, strip.Color(currentR / 3, currentG / 3, currentB / 3));
    }
    else if (distance == 2) {
      strip.setPixelColor(i, strip.Color(currentR / 8, currentG / 8, currentB / 8));
    }
  }

  strip.show();

  waterIndex++;
  if (waterIndex >= NUMPIXELS) waterIndex = 0;
}

// --------------------------------------------------
// METEOR - SEÇİLEN RENKLE
// --------------------------------------------------

void effectMeteorStep() {
  strip.clear();

  for (int tail = 0; tail < 6; tail++) {
    int idx = meteorIndex - tail;

    if (idx >= 0 && idx < NUMPIXELS) {
      int brightness = 255 - tail * 40;
      if (brightness < 0) brightness = 0;

      uint8_t r = (currentR * brightness) / 255;
      uint8_t g = (currentG * brightness) / 255;
      uint8_t b = (currentB * brightness) / 255;

      strip.setPixelColor(idx, strip.Color(r, g, b));
    }
  }

  strip.show();

  meteorIndex++;
  if (meteorIndex >= NUMPIXELS + 6) meteorIndex = 0;
}

// --------------------------------------------------
// PULSE - SEÇİLEN RENKLE
// --------------------------------------------------

void effectPulseStep() {
  float s = (sin(pulsePhase) + 1.0) / 2.0;   // 0..1
  int brightness = 30 + s * 225;            // 30..255

  uint8_t r = (currentR * brightness) / 255;
  uint8_t g = (currentG * brightness) / 255;
  uint8_t b = (currentB * brightness) / 255;

  setAllPixels(r, g, b);

  pulsePhase += 0.18;
  if (pulsePhase > 6.28) pulsePhase = 0.0;
}

// --------------------------------------------------
// BREATH - SEÇİLEN RENKLE
// --------------------------------------------------

void effectBreathStep() {
  float s = (sin(breathPhase) + 1.0) / 2.0;   // 0..1
  int brightness = 5 + s * 180;              // daha yumuşak

  uint8_t r = (currentR * brightness) / 255;
  uint8_t g = (currentG * brightness) / 255;
  uint8_t b = (currentB * brightness) / 255;

  setAllPixels(r, g, b);

  breathPhase += 0.08;
  if (breathPhase > 6.28) breathPhase = 0.0;
}

// --------------------------------------------------
// WAVE - SEÇİLEN RENKLE
// --------------------------------------------------

void effectWaveStep() {
  for (int i = 0; i < NUMPIXELS; i++) {
    float wave = (sin((i * 0.5) + waveOffset) + 1.0) / 2.0; // 0..1
    int brightness = 20 + wave * 235;

    uint8_t r = (currentR * brightness) / 255;
    uint8_t g = (currentG * brightness) / 255;
    uint8_t b = (currentB * brightness) / 255;

    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();

  waveOffset += 0.25;
  if (waveOffset > 6.28) waveOffset = 0.0;
}

// --------------------------------------------------
// RENK SEÇİMİ
// --------------------------------------------------

void setCurrentColor(uint8_t r, uint8_t g, uint8_t b) {
  currentR = r;
  currentG = g;
  currentB = b;
}

void handleColorRed()    { setCurrentColor(255, 0, 0);   server.send(200, "text/plain", "Color: red"); }
void handleColorBlue()   { setCurrentColor(0, 0, 255);   server.send(200, "text/plain", "Color: blue"); }
void handleColorGreen()  { setCurrentColor(0, 255, 0);   server.send(200, "text/plain", "Color: green"); }
void handleColorYellow() { setCurrentColor(255, 255, 0); server.send(200, "text/plain", "Color: yellow"); }
void handleColorOrange() { setCurrentColor(255, 80, 0);  server.send(200, "text/plain", "Color: orange"); }
void handleColorPurple() { setCurrentColor(180, 0, 255); server.send(200, "text/plain", "Color: purple"); }
void handleColorWhite()  { setCurrentColor(255, 255, 255); server.send(200, "text/plain", "Color: white"); }

// --------------------------------------------------
// MOD SEÇİMİ
// --------------------------------------------------

void setMode(String modeName) {
  currentMode = modeName;
  resetAnimationStates();
}

void handleRoot()         { server.send(200, "text/plain", "ESP32 Pixel Controller Ready"); }
void handleModeOff()      { setMode("off");      server.send(200, "text/plain", "Mode: off"); }
void handleModeSolid()    { setMode("solid");    server.send(200, "text/plain", "Mode: solid"); }
void handleModeRainbow()  { setMode("rainbow");  server.send(200, "text/plain", "Mode: rainbow"); }
void handleModePolice()   { setMode("police");   server.send(200, "text/plain", "Mode: police"); }
void handleModeWater()    { setMode("water");    server.send(200, "text/plain", "Mode: water"); }
void handleModeMeteor()   { setMode("meteor");   server.send(200, "text/plain", "Mode: meteor"); }
void handleModePulse()    { setMode("pulse");    server.send(200, "text/plain", "Mode: pulse"); }
void handleModeBreath()   { setMode("breath");   server.send(200, "text/plain", "Mode: breath"); }
void handleModeWave()     { setMode("wave");     server.send(200, "text/plain", "Mode: wave"); }

// --------------------------------------------------
// SETUP
// --------------------------------------------------

void setup() {
  Serial.begin(115200);

  strip.begin();
  strip.setBrightness(50);
  strip.clear();
  strip.show();

  WiFi.softAP(ssid, password);

  server.on("/", handleRoot);

  // renkler
  server.on("/color/red", handleColorRed);
  server.on("/color/blue", handleColorBlue);
  server.on("/color/green", handleColorGreen);
  server.on("/color/yellow", handleColorYellow);
  server.on("/color/orange", handleColorOrange);
  server.on("/color/purple", handleColorPurple);
  server.on("/color/white", handleColorWhite);

  // modlar
  server.on("/mode/off", handleModeOff);
  server.on("/mode/solid", handleModeSolid);
  server.on("/mode/rainbow", handleModeRainbow);
  server.on("/mode/police", handleModePolice);
  server.on("/mode/water", handleModeWater);
  server.on("/mode/meteor", handleModeMeteor);
  server.on("/mode/pulse", handleModePulse);
  server.on("/mode/breath", handleModeBreath);
  server.on("/mode/wave", handleModeWave);

  server.begin();
}

// --------------------------------------------------
// LOOP
// --------------------------------------------------

void loop() {
  server.handleClient();

  unsigned long now = millis();

  if (currentMode == "off") {
    effectOff();
    delay(20);
  }
  else if (currentMode == "solid") {
    effectSolid();
    delay(20);
  }
  else if (currentMode == "rainbow") {
    if (now - lastUpdate > 30) {
      effectRainbowStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "police") {
    if (now - lastUpdate > 180) {
      effectPoliceStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "water") {
    if (now - lastUpdate > 90) {
      effectWaterStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "meteor") {
    if (now - lastUpdate > 60) {
      effectMeteorStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "pulse") {
    if (now - lastUpdate > 35) {
      effectPulseStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "breath") {
    if (now - lastUpdate > 35) {
      effectBreathStep();
      lastUpdate = now;
    }
  }
  else if (currentMode == "wave") {
    if (now - lastUpdate > 45) {
      effectWaveStep();
      lastUpdate = now;
    }
  }
}
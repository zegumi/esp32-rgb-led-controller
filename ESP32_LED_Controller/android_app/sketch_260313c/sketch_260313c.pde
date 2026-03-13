import java.net.Socket;
import java.io.PrintWriter;

String espIP = "192.168.4.1";

String currentPath = "";
String durumMesaji = "Hazir";
String selectedColorName = "RED";
String selectedEffectName = "OFF";

void setup() {
  fullScreen();
  textAlign(CENTER, CENTER);
  rectMode(CENTER);
}

void draw() {
  background(18, 22, 35);

  fill(255);
  textSize(36);
  text("ESP32 PIXEL KONTROL", width/2, 80);

  textSize(24);
  text("Renkler", width/2, 150);

  // RENK BUTONLARI
  drawButton(width/4, 240, 220, 90, "RED", color(220, 40, 40));
  drawButton(width*3/4, 240, 220, 90, "BLUE", color(40, 100, 220));

  drawButton(width/4, 360, 220, 90, "GREEN", color(40, 180, 70));
  drawButton(width*3/4, 360, 220, 90, "YELLOW", color(240, 220, 40));

  drawButton(width/4, 480, 220, 90, "ORANGE", color(255, 120, 0));
  drawButton(width*3/4, 480, 220, 90, "PURPLE", color(170, 0, 255));

  drawButton(width/2, 600, 260, 90, "WHITE", color(180));

  fill(255);
  textSize(24);
  text("Efektler", width/2, 710);

  // EFEKT BUTONLARI
  drawButton(width/4, 800, 220, 90, "OFF", color(90));
  drawButton(width*3/4, 800, 220, 90, "SOLID", color(120));

  drawButton(width/4, 920, 220, 90, "METEOR", color(160));
  drawButton(width*3/4, 920, 220, 90, "WATER", color(0, 150, 255));

  drawButton(width/4, 1040, 220, 90, "PULSE", color(255, 80, 120));
  drawButton(width*3/4, 1040, 220, 90, "BREATH", color(0, 200, 180));

  drawButton(width/4, 1160, 220, 90, "WAVE", color(0, 120, 255));
  drawButton(width*3/4, 1160, 220, 90, "RAINBOW", color(180, 0, 220));

  drawButton(width/2, 1280, 260, 90, "POLICE", color(100, 0, 220));

  fill(255);
  textSize(20);
  text("Secilen Renk: " + selectedColorName, width/2, height - 120);
  text("Secilen Efekt: " + selectedEffectName, width/2, height - 90);
  text("Durum: " + durumMesaji, width/2, height - 60);
}

void drawButton(float x, float y, float w, float h, String label, int c) {
  fill(c);
  rect(x, y, w, h, 18);

  fill(255);
  textSize(24);
  text(label, x, y);
}

boolean insideButton(float mx, float my, float x, float y, float w, float h) {
  return mx > x - w/2 && mx < x + w/2 && my > y - h/2 && my < y + h/2;
}

void mousePressed() {

  // ------------------------------
  // RENKLER
  // ------------------------------
  if (insideButton(mouseX, mouseY, width/4, 240, 220, 90)) {
    selectedColorName = "RED";
    sendCommand("/color/red");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 240, 220, 90)) {
    selectedColorName = "BLUE";
    sendCommand("/color/blue");
  }
  else if (insideButton(mouseX, mouseY, width/4, 360, 220, 90)) {
    selectedColorName = "GREEN";
    sendCommand("/color/green");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 360, 220, 90)) {
    selectedColorName = "YELLOW";
    sendCommand("/color/yellow");
  }
  else if (insideButton(mouseX, mouseY, width/4, 480, 220, 90)) {
    selectedColorName = "ORANGE";
    sendCommand("/color/orange");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 480, 220, 90)) {
    selectedColorName = "PURPLE";
    sendCommand("/color/purple");
  }
  else if (insideButton(mouseX, mouseY, width/2, 600, 260, 90)) {
    selectedColorName = "WHITE";
    sendCommand("/color/white");
  }

  // ------------------------------
  // EFEKTLER
  // ------------------------------
  else if (insideButton(mouseX, mouseY, width/4, 800, 220, 90)) {
    selectedEffectName = "OFF";
    sendCommand("/mode/off");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 800, 220, 90)) {
    selectedEffectName = "SOLID";
    sendCommand("/mode/solid");
  }
  else if (insideButton(mouseX, mouseY, width/4, 920, 220, 90)) {
    selectedEffectName = "METEOR";
    sendCommand("/mode/meteor");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 920, 220, 90)) {
    selectedEffectName = "WATER";
    sendCommand("/mode/water");
  }
  else if (insideButton(mouseX, mouseY, width/4, 1040, 220, 90)) {
    selectedEffectName = "PULSE";
    sendCommand("/mode/pulse");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 1040, 220, 90)) {
    selectedEffectName = "BREATH";
    sendCommand("/mode/breath");
  }
  else if (insideButton(mouseX, mouseY, width/4, 1160, 220, 90)) {
    selectedEffectName = "WAVE";
    sendCommand("/mode/wave");
  }
  else if (insideButton(mouseX, mouseY, width*3/4, 1160, 220, 90)) {
    selectedEffectName = "RAINBOW";
    sendCommand("/mode/rainbow");
  }
  else if (insideButton(mouseX, mouseY, width/2, 1280, 260, 90)) {
    selectedEffectName = "POLICE";
    sendCommand("/mode/police");
  }
}

void sendCommand(String path) {
  currentPath = path;
  durumMesaji = "Gonderiliyor: " + path;
  thread("requestThread");
}

void requestThread() {
  try {
    Socket socket = new Socket(espIP, 80);
    PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

    out.print("GET " + currentPath + " HTTP/1.1\r\n");
    out.print("Host: " + espIP + "\r\n");
    out.print("Connection: close\r\n\r\n");
    out.flush();

    out.close();
    socket.close();

    durumMesaji = "Basarili: " + currentPath;
  } 
  catch (Exception e) {
    durumMesaji = "Hata: " + e.getMessage();
    println(e);
  }
}

#include <U8glib.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// DFPlayer Mini
SoftwareSerial dfSerial(10, 11); // RX, TX (adjust as needed)
DFRobotDFPlayerMini dfplayer;

// Switch pins
const int switch1Pin = 2; // Language scroll
const int switch2Pin = 3; // Play track
const int switch3Pin = 4; // Volume toggle

// Languages & tracks
const int numberOfLanguages = 5;
const char* languages[numberOfLanguages] = {
  "German",
  "English",
  "French",
  "Spanish",
  "Italian"
};

// Current index of the active language (middle)
int scrollIndex = 0;
bool volumeOn = true;

// Last known switch states
bool lastStateSwitch1;
bool lastStateSwitch2;
bool lastStateSwitch3;

void setup() {
  Serial.begin(9600);
  u8g.setFont(u8g_font_ncenB08);

  // Setup switches
  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP);
  pinMode(switch3Pin, INPUT_PULLUP);

  lastStateSwitch1 = digitalRead(switch1Pin);
  lastStateSwitch2 = digitalRead(switch2Pin);
  lastStateSwitch3 = digitalRead(switch3Pin);

  // DFPlayer Setup
  dfSerial.begin(9600);
  if (!dfplayer.begin(dfSerial)) {
    Serial.println("DFPlayer not found!");
  } else {
    Serial.println("DFPlayer ready.");
    dfplayer.volume(20); // Start volume (0–30)
  }
}

void loop() {
  bool currentStateSwitch1 = digitalRead(switch1Pin);
  bool currentStateSwitch2 = digitalRead(switch2Pin);
  bool currentStateSwitch3 = digitalRead(switch3Pin);

  // Scroll language
  if (currentStateSwitch1 != lastStateSwitch1) {
    scrollIndex = (scrollIndex + 1) % numberOfLanguages;
    Serial.print("New active language: ");
    Serial.println(languages[scrollIndex]);
  }
  lastStateSwitch1 = currentStateSwitch1;

  // Play track
  if (currentStateSwitch2 != lastStateSwitch2) {
    int track = scrollIndex + 1; // Language 0 → Track 1 etc.
    Serial.print("Playing track ");
    Serial.println(track);
    dfplayer.play(track);
  }
  lastStateSwitch2 = currentStateSwitch2;

  // Toggle volume
  if (currentStateSwitch3 != lastStateSwitch3) {
    volumeOn = !volumeOn;
    if (volumeOn) {
      dfplayer.volume(20); // Loud
      Serial.println("Volume: LOUD");
    } else {
      dfplayer.volume(5); // Quiet
      Serial.println("Volume: QUIET");
    }
  }
  lastStateSwitch3 = currentStateSwitch3;

  // Update display
  u8g.firstPage();
  do {
    int yPositions[3] = {18, 34, 50};

    for (int i = 0; i < 3; i++) {
      int index = (scrollIndex + i - 1 + numberOfLanguages) % numberOfLanguages;
      const char* text = languages[index];

      int x = (128 - u8g.getStrWidth(text)) / 2;
      int y = yPositions[i];

      u8g.drawStr(x, y, text);

      if (i == 1) {
        u8g.drawFrame(x - 2, y - 10, u8g.getStrWidth(text) + 4, 12);
      }
    }

    // Move speaker symbol higher
    int baseX = 112;
    int baseY = 46;

    u8g.drawBox(baseX, baseY + 2, 3, 8);
    u8g.drawLine(baseX + 3, baseY + 2, baseX + 6, baseY);
    u8g.drawLine(baseX + 3, baseY + 10, baseX + 6, baseY + 12);
    u8g.drawLine(baseX + 6, baseY, baseX + 6, baseY + 12);

    if (volumeOn) {
      u8g.drawLine(baseX + 8, baseY + 3, baseX + 10, baseY + 1);
      u8g.drawLine(baseX + 8, baseY + 9, baseX + 10, baseY + 11);
      u8g.drawLine(baseX + 10, baseY + 1, baseX + 10, baseY + 11);
    } else {
      u8g.drawLine(baseX + 8, baseY + 2, baseX + 12, baseY + 10);
    }

  } while (u8g.nextPage());

  delay(20);
}

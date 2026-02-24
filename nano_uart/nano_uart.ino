#include <SoftwareSerial.h>

// Pins: D8 = RX (← CrowPanel TX), D9 = TX (→ CrowPanel RX)
SoftwareSerial commSerial(8, 9);  // RX, TX

void setup() {
  Serial.begin(115200);       // USB debugging
  commSerial.begin(115200);   // To CrowPanel
  delay(300);

  Serial.println("Arduino Nano dummy sender started (battery/electrical style)");
  Serial.println("Sending 10 lines every ~1.2s → indices 0..9");
}

void loop() {
  static int cycle = 0;
  char buf[32];
  char floatStr[10];

  commSerial.println("\n");

  // ─────────────────────────────────────────────
  // 0: Temperature (main / ambient sensor)
  //13.4 + (cycle % 120) * 0.02
  dtostrf(13.4 + (cycle % 120) * 0.02, 4, 2, floatStr); 
  snprintf(buf, sizeof(buf), "A%s°", floatStr);  // 18.5 → 46.5 °C
  commSerial.println(buf);
  Serial.println(buf);

  
  // 1: Ambience (feels like / secondary temp or comfort)
  dtostrf(17.2 + (cycle % 220) * 0.1, 4, 2, floatStr); 
  snprintf(buf, sizeof(buf), "B%s", floatStr);
  commSerial.println(buf);
  Serial.println(buf);
  // 2: Voltage (main bus / system voltage)
  dtostrf(12.1 + (cycle % 180) * 0.01, 4, 2, floatStr); 
  snprintf(buf, sizeof(buf), "C%sV", floatStr);  // ~11.8–13.0 V
  commSerial.println(buf);
  Serial.println(buf);
  // 3: Ampere (current draw / load)
  dtostrf(-2.4 + (cycle % 140) * 0.1, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "D%samp", floatStr);   // -2.4 … +11.5 A
  commSerial.println(buf);
  Serial.println(buf);
  // 4: Voltage (battery voltage)
  dtostrf(3.68 + (cycle % 64) * 0.01, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "E%sV", floatStr);   // ~3.68–4.32 V per cell style
  commSerial.println(buf);
  Serial.println(buf);
  // 5: State of Charge (%)
  snprintf(buf, sizeof(buf), "F%d%%", 22 + (cycle % 78));
  commSerial.println(buf);
  Serial.println(buf);
  // 6: Time left (seconds)
  long secondsLeft = 3600L + (cycle % 14400L) - (cycle % 7200L);  // jumps between ~0–4 hours
  snprintf(buf, sizeof(buf), "G%lds", secondsLeft);
  commSerial.println(buf);
  Serial.println(buf);
  // 7: Voltage (another reading – e.g. charger / alternator)
  dtostrf(13.4 + (cycle % 120) * 0.02, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "H%sV", floatStr);  // 13.4–15.8 V
  commSerial.println(buf);
  Serial.println(buf);
  // 8: Ampere (charging current / another circuit)
  dtostrf(0.0 + (cycle % 80) * 0.3, 4, 2, floatStr);
  snprintf(buf, sizeof(buf), "I%samp", floatStr);     // 0.0–23.7 A
  commSerial.println(buf);
  Serial.println(buf);
  // 9: State of Charge (second instance – maybe different pack)
  int soc2 = 15 + ((cycle + 37) % 85);   // offset cycle so it's different
  snprintf(buf, sizeof(buf), "J%d%%", soc2);
  commSerial.println(buf);
  Serial.println(buf);
  // ─────────────────────────────────────────────
  // Optional: show what was sent (for debugging)
  
  cycle++;
  delay(1200);   // ≈1.2 seconds per full update – change to 500–2000 ms as needed
}
T-Display S3 external LED blink schematic
=========================================

Circuit:
  T-Display S3 GPI13 -- 330 ohm resistor -- LED -- GND

Files:
  TDisplayS3_LED_Blink.sch
    KiCad legacy schematic format. KiCad 7/8/9 can open/import it and then
    save it in the current .kicad_sch format.

Physical connection on T-Display S3:
  GPI13: left-side expansion header, GPI13 pin
  GND:   any GND pin on the expansion header

LED polarity:
  Anode (+, long lead)    -> R1 / GPI13 side
  Cathode (-, short lead) -> GND side

Suggested Arduino test code:

  constexpr int LED_PIN = 13;

  void setup() {
      pinMode(LED_PIN, OUTPUT);
  }

  void loop() {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
  }

Notes:
- GPIO3 is selected because it is exposed on the standard T-Display S3 header
  and is suitable for general GPIO use.
- R1 = 330 ohm is a conservative general-purpose value for a 3.3 V GPIO LED.
- If using a specific LED with unusual forward voltage/current requirements,
  recalculate R1 as needed.

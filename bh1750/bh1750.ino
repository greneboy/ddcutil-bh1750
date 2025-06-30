#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Wire.begin();
  lightMeter.begin();
}

void loop() {

  // Wait for serial in
  if (Serial.available() > 0) {

    float lux = lightMeter.readLightLevel();

    // Read it
    char c = Serial.read();

    if ( c == '\r' ) {
      Serial.println(lux);
    }
  }
}



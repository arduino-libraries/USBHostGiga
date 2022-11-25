#include "HIDHost.h"

//REDIRECT_STDOUT_TO(Serial)
Keyboard keyb;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  pinMode(PA_15, OUTPUT);
  keyb.begin();
}


void loop() {
  if (keyb.available()) {
    Serial.println(keyb.read());
  }
}

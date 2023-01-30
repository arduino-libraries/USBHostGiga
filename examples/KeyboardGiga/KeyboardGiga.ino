#include "USBHostGiga.h"

//REDIRECT_STDOUT_TO(Serial)
Keyboard keyb;
HostSerial ser;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  pinMode(PA_15, OUTPUT);
  keyb.begin();
  ser.begin();
}


void loop() {
  if (keyb.available()) {
    auto _key = keyb.read();
    Serial.println(keyb.getAscii(_key));
  }
  while (ser.available()) {
    auto _char = ser.read();
    Serial.write(_char);
  }
  //delay(1);
}
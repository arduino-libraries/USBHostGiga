#include "Arduino.h"
#include "usb_host.h"
#include "usbh_def.h"

class Keyboard {
public:
    void begin();
    size_t available();
    char read();
    static RingBuffer rxBuffer;
};
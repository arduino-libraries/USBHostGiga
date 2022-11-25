#include "HIDHost.h"
#include "usbh_hid_keybd.h"

RingBuffer Keyboard::rxBuffer;

extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
    Keyboard::rxBuffer.store_char(USBH_HID_GetASCIICode(USBH_HID_GetKeybdInfo(phost)));
}

extern "C" void Error_Handler() {}

size_t Keyboard::available() {
    return rxBuffer.available();
}

char Keyboard::read() {
    return rxBuffer.read_char();
}

void Keyboard::begin() {
    MX_USB_HOST_Init();
}
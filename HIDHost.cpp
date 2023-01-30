#include "HIDHost.h"

RingBufferNGeneric<64, HID_KEYBD_Info_TypeDef> Keyboard::rxBuffer;
RingBufferNGeneric<64, HID_MOUSE_Info_TypeDef> Mouse::rxBuffer;
RingBuffer HostSerial::rxBuffer;

extern "C" void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
    auto kbd_evt = USBH_HID_GetKeybdInfo(phost);
    if (kbd_evt != NULL) {
        Keyboard::rxBuffer.store_elem(*kbd_evt);
    }
    auto mouse_evt = USBH_HID_GetMouseInfo(phost);
    if (mouse_evt != NULL) {
        Mouse::rxBuffer.store_elem(*mouse_evt);
    }
    //Keyboard::rxBuffer.store_char(USBH_HID_GetASCIICode());
}

extern "C" void Error_Handler() {}

size_t Keyboard::available() {
    return rxBuffer.available();
}

HID_KEYBD_Info_TypeDef Keyboard::read() {
    return rxBuffer.read_elem();
}

char Keyboard::getAscii(HID_KEYBD_Info_TypeDef evt) {
    return USBH_HID_GetASCIICode(&evt);
}

void Keyboard::begin() {
    MX_USB_HOST_Init();
}

size_t Mouse::available() {
    return rxBuffer.available();
}

HID_MOUSE_Info_TypeDef Mouse::read() {
    return rxBuffer.read_elem();
}

void Mouse::begin() {
    MX_USB_HOST_Init();
}

extern "C" USBH_HandleTypeDef hUsbHostHS;

static uint8_t buf[64];
extern "C" void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef *phost) {    
    //USBH_UsrLog("USBH_CDC_Receive");
    HostSerial::rxBuffer.store_char(buf[0]);
}

extern "C" ApplicationTypeDef Appli_state;

void HostSerial::begin(unsigned long unused, uint16_t config) {
    MX_USB_HOST_Init();
    while (Appli_state != APPLICATION_READY) {
        Serial.println(Appli_state);
        delay(100);
    }
    USBH_ErrLog("HostSerial::begin 1");

    static CDC_LineCodingTypeDef linecoding;
    linecoding.b.dwDTERate = 115200;
    linecoding.b.bDataBits = 8;
    USBH_CDC_SetLineCoding(&hUsbHostHS, &linecoding);
}

int HostSerial::available() {
    USBH_CDC_Stop(&hUsbHostHS);
    USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
    return rxBuffer.available();
}

int HostSerial::read() {
    return rxBuffer.read_char();
}

size_t HostSerial::write(uint8_t) {
        //USBH_CDC_Transmit()
};
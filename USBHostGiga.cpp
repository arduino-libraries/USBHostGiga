#include "USBHostGiga.h"

RingBufferNGeneric<64, HID_KEYBD_Info_TypeDef> Keyboard::rxBuffer;
RingBufferNGeneric<64, HID_MOUSE_Info_TypeDef> Mouse::rxBuffer;

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

HostSerial* _hostSerial = nullptr;
static uint8_t buf[64];

extern "C" void USBH_CDC_ReceiveCallback(USBH_HandleTypeDef* phost) {    
    _hostSerial->rx_cb(buf, sizeof(buf) + USBH_CDC_GetLastReceivedDataSize(phost));
    //USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
}

void HostSerial::rx_cb(uint8_t* data, size_t len) {
    _mut.lock();
    for (int i = 0; i < len; i++) {
        rxBuffer.store_char(data[i]);
    }
    _mut.unlock();
}

extern "C" ApplicationTypeDef Appli_state;

void HostSerial::begin(unsigned long unused, uint16_t config) {
    MX_USB_HOST_Init();
    while (Appli_state != APPLICATION_READY) {
        delay(100);
    }
    _hostSerial = this;

    static CDC_LineCodingTypeDef linecoding;
    linecoding.b.dwDTERate = 115200;
    linecoding.b.bDataBits = 8;
    USBH_CDC_SetLineCoding(&hUsbHostHS, &linecoding);
    USBH_CDC_SetControlLineState(&hUsbHostHS, 1, 1);
    USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
}

int HostSerial::available() {
    //USBH_CDC_Stop(&hUsbHostHS);
    _mut.lock();
    auto ret = rxBuffer.available();
    if (ret == 0) {
        USBH_CDC_Receive(&hUsbHostHS, buf, sizeof(buf));
    }
    _mut.unlock();
    return ret;
}

int HostSerial::read() {
    _mut.lock();
    auto ret = rxBuffer.read_char();
    _mut.unlock();
    return ret;
}

size_t HostSerial::write(uint8_t) {
        //USBH_CDC_Transmit()
};
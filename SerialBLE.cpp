#include "SerialBLE.h"
#include "ArduinoBLE.h"

static BLEService uartService("0000ffe0-0000-1000-8000-00805f9b34fb"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
static BLECharacteristic uartCharacteristic("0000ffe1-0000-1000-8000-00805f9b34fb", BLEWrite | BLENotify, 20);

void SerialBLEClass::loop() {
    while (1) {

        BLEDevice central = BLE.central();

        // if a central is connected to peripheral:
        if (central) {

            while (central.connected()) {

                if (uartCharacteristic.written()) {
                    int length = uartCharacteristic.valueLength();
                    const uint8_t* val = uartCharacteristic.value();
                    for (int i = 0; i < length; i++) {
                        rx_buffer.store_char((char)val[i]);
                    }
                }

                int available = tx_buffer.available();
                if (available) {
                    uint8_t buf[available];
                    int i = 0;
                    while (i < available) {
                        buf[i++] = tx_buffer.read_char();
                    }
                    uartCharacteristic.writeValue(buf, available);
                }
            }
        } else {
            yield();
        }
    }
}

void SerialBLEClass::begin(unsigned long baudrate) {
    begin(baudrate, 0);
}

void SerialBLEClass::begin(unsigned long baudrate, uint16_t config) {
    if (!BLE.begin()) {
        return;
    }

    // set advertised local name and service UUID:
    BLE.setLocalName("UART");
    BLE.setAdvertisedService(uartService);

    // add the characteristic to the service
    uartService.addCharacteristic(uartCharacteristic);

    // add service
    BLE.addService(uartService);

    // start advertising
    BLE.advertise();

    rx_buffer.clear();
    tx_buffer.clear();

    thd.start(mbed::callback(this, &SerialBLEClass::loop));
}

void SerialBLEClass::end() {
    BLE.end();
}
int SerialBLEClass::available(void) {
    return rx_buffer.available();
}

int SerialBLEClass::peek(void) {
    return rx_buffer.peek();
}
int SerialBLEClass::read(void) {
    return rx_buffer.read_char();
}
void SerialBLEClass::flush(void) {
    rx_buffer.clear();
    tx_buffer.clear();
}

size_t SerialBLEClass::write(uint8_t c) {
    tx_buffer.store_char(c);
    return 1;
}

SerialBLEClass::operator bool() {
    BLEDevice central = BLE.central();
    if (!central || !central.connected()) {
        return false;
    }
    return true; 
}

SerialBLEClass SerialBLE;
#include "Arduino.h"
#include "mbed.h"

class SerialBLEClass : public arduino::HardwareSerial {
public:
    virtual void begin(unsigned long baudrate);
    virtual void begin(unsigned long baudrate, uint16_t config);
    virtual void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    virtual operator bool();
private:
  	void loop();
  	RingBufferN<256> tx_buffer;
	RingBufferN<256> rx_buffer;
	rtos::Thread thd;
};

extern SerialBLEClass SerialBLE;
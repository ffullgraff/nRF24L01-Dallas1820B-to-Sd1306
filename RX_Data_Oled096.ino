
/*
CE    -> 10
CSN   -> 9 (Hardware SPI SS)
MOSI  -> 11 (Hardware SPI MOSI)
MISO  -> 12 (Hardware SPI MISO)
SCK   -> 13 (Hardware SPI SCK)
IRQ   -> No connection
VCC   -> No more than 3.6 volts
GND   -> GND

*/

#include <SPI.h>
#include <NRFLite.h>
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

const static uint8_t RADIO_ID = 0;       // Our radio's id.  The transmitter will send to this id.
const static uint8_t PIN_RADIO_CE = 10;
const static uint8_t PIN_RADIO_CSN = 9;


struct TempPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    float Temp;
};


NRFLite _radio;
TempPacket _tempData;

void setup()
{
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);

  uint32_t m = micros();
    Serial.begin(115200);
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
}

void loop()
{
    while (_radio.hasData())
    {
      oled.clear();
  oled.println("Temperatura Refrigarado 1 !");
   oled.println("-------------------------- !");
  
        _radio.readData(&_tempData); // Note how '&' must be placed in front of the variable name.

        String msg = "Radio ";
        msg += _tempData.FromRadioId;
        msg += ", ";
        msg += _tempData.Temp;
        msg += " °C ";
        oled.println(msg);
        delay(1000);
        oled.clear();
    }
}

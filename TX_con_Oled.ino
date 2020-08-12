#include <SPI.h>
#include <NRFLite.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

//DS18B20
#define ONE_WIRE_BUS 4 //Pin to which is attached a temperature sensor + 4k7 resistor to 3V3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const static uint8_t RADIO_ID = 1;             // Our radio's id.
const static uint8_t DESTINATION_RADIO_ID = 0; // Id of the radio we will transmit to.
const static uint8_t PIN_RADIO_CE = 10;
const static uint8_t PIN_RADIO_CSN = 9;

struct TempPacket // Any packet up to 32 bytes can be sent.
{
    uint8_t FromRadioId;
    float Temp;        //Only uint8_t and int8_t works with ESP8266
};

NRFLite _radio;
TempPacket _tempData;


void setup() {
    Serial.begin(115200);
    #if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);

  uint32_t m = micros();
  oled.clear();
    if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN))
    {
        Serial.println("Cannot communicate with radio");
        while (1); // Wait here forever.
    }
   
    _tempData.FromRadioId = RADIO_ID;
    sensors.begin();

}

void loop() {
    sensors.requestTemperatures();
    _tempData.Temp = sensors.getTempCByIndex(0);
   
    oled.print("Sending temperature");
    float tempC = sensors.getTempCByIndex(0);

    if (_radio.send(DESTINATION_RADIO_ID, &_tempData, sizeof(_tempData))) // Note how '&' must be placed in front of the variable name.
    {
        oled.println("...Success");
    }
    else
    {
        oled.println("...Failed");
 
    }
 oled.println(tempC);
    delay(1000);
}

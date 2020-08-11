#include <SPI.h>
#include <NRFLite.h>

#include <OneWire.h>
#include <DallasTemperature.h>

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
   
    Serial.print("Sending temperature");

    if (_radio.send(DESTINATION_RADIO_ID, &_tempData, sizeof(_tempData))) // Note how '&' must be placed in front of the variable name.
    {
        Serial.println("...Success");
    }
    else
    {
        Serial.println("...Failed");
    }

    delay(1000);
}

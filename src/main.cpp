#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_BME280.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <RFM69.h>
#include <RFM69_ATC.h>
#include <RFM69_OTA.h>
#include <SparkFun_Ublox_Arduino_Library.h>


#define BME_SCK      13
#define BME_MISO     12
#define BME_MOSI     11
#define BME_CS       10
#define SEALEVELPRESSURE_HPA (1013.25)
#define DELAYVAL     500     // Time (in milliseconds) to pause between pixels
#define DHTPIN       5       // Digital pin connected to the DHT sensor
#define DHTTYPE      DHT11
#define PHOTOSENSOR  A3

#define NODEID           789
#define NETWORKID        100
#define GATEWAYID        1   //as a rule of thumb the gateway ID should always be 1
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW_HCW  //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
//*****************************************************************************************************************************
#define ENABLE_ATC      //comment out this line to disable AUTO TRANSMISSION CONTROL
#define ATC_RSSI        -90

#ifdef ENABLE_ATC
  RFM69_ATC radio;
#else
  RFM69 radio;
#endif

Adafruit_INA219   ina219;
Adafruit_BME280   bme; // I2C
DHT               dht(DHTPIN, DHTTYPE);
SFE_UBLOX_GPS     myGPS;


int HighByte, LowByte, TReading, SignBit, Tc_100, Fh_100, Whole, Fract;
char buff[61]; //max packet size is 61 with encryption enabled
char Fstr[10];
byte buffLen;
float fahrenheit, celsius;
uint32_t time=0, now=0, LASTPACKETTIME=-1;
uint16_t conversionTime=0;


bool radio_init()
{
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  #ifdef FREQUENCY_EXACT
    radio.setFrequency(FREQUENCY_EXACT); //set frequency to some custom frequency
  #endif
  radio.encrypt(ENCRYPTKEY);

  #ifdef ENABLE_ATC
    radio.enableAutoPower(ATC_RSSI);
  #endif

  radio.sleep();
  return true;
}

bool GPS_init()
{
  if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
  {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    return false;
  }

  myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfiguration(); //Save the current settings to flash and BBR
  return true;
}



void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  
}

void loop() 
{

}
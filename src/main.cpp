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
#include <Adafruit_SleepyDog.h>


#define SCK          PB5
#define MISO         PB4
#define MOSI         PB3
#define SS           PB2
#define SEALEVELPRESSURE_HPA (1013.25)
#define DELAYVAL     500     // Time (in milliseconds) to pause between pixels
#define DHTPIN       PD5       // Digital pin connected to the DHT sensor
#define DHTTYPE      DHT11
#define PHOTOSENSOR  PC3
#define BUZZER       PD3
#define SEL0         PD4
#define SEL1         PE0
#define U5V_ENABLE   PC2

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


uint16_t SPI_data = 0;

ISR (SPI0_STC_vect)   //Inerrrput routine function
{
  SPI_data = SPDR;
}

void spi_init()
{
  pinMode(SS, INPUT_PULLUP);
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, INPUT);
  SPCR0 |= _BV(SPE);
  SPI.attachInterrupt();  //allows SPI interrupt
}

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


void setup() 
{
  Serial.begin(115200);
  Wire.begin();
  spi_init();
  pinMode(SEL0, INPUT_PULLUP);
  pinMode(SEL1, INPUT_PULLUP);
  pinMode(PHOTOSENSOR, INPUT);

  bme.init();
  ina219.begin();
  radio_init();

  tone(BUZZER, 1000, 100);
}

void loop() 
{

}
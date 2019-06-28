#include <SPI.h>
#include <RH_RF95.h>
#include "DHT.h"
#define DHTPIN 3
#define DHTTYPE DHT11

int ac = 4;
int client = 3;

unsigned long previousMillis = 0;
const long interval = 30000;

RH_RF95 rf95;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  Serial.println("Start LoRa Client");
  if (!rf95.init())
    Serial.println("init failed");
  rf95.setFrequency(868.0);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
  rf95.setSyncWord(0x34);
  pinMode(ac, OUTPUT);
  digitalWrite(ac, HIGH);
  sendUpdates();
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendUpdates();
  }
  receiveCommand();
}
void receiveCommand() {
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len))
      if (buf[0] - 48 == client) {
        String relay = String(buf[1] - 48);
        digitalWrite(ac, !relay.toInt());
        delay(300);
        sendUpdates();
      }
  }
}
void sendUpdates() {
  String temp = String(client);
  temp += String(getACstate());
  temp += String(getSensorData());
  uint8_t data[temp.length() + 1];
  temp.getBytes(data, temp.length() + 1);
  rf95.send(data, temp.length());
  rf95.waitPacketSent();
}
float getSensorData() {
  float sensorValue = dht.readTemperature();
  Serial.println(sensorValue);
  return sensorValue;
}
int getACstate() {
  int acState = !digitalRead(ac);
  return acState;
}

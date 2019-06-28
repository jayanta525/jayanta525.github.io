#include <SPI.h>
#include <RH_RF95.h>

int ac = 4;
int client = 2;

unsigned long previousMillis = 0;
const long interval = 30000;

RH_RF95 rf95;

void setup()
{
  Serial.begin(9600);
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
    {
      Serial.print("Client: ");
      Serial.println(buf[0] - 48);
      if (buf[0] - 48 == client) {
        Serial.print("Relay Status: ");
        Serial.println(buf[1] - 48);
        String relay = String(buf[1] - 48);
        digitalWrite(ac, relay.toInt());
        delay(300);
        sendUpdates();
      }
      else
        Serial.println("Message Not for Me");
      //send ACK
//            uint8_t data[] = "Confirmed";
//            rf95.send(data, sizeof(data));
//            rf95.waitPacketSent();
//            Serial.println("Command ACK Sent");
      //sendUpdates();
    }
  }
}
void sendUpdates() {
  Serial.println("Sending to LoRa Server");
  String temp = String(client);
  temp += String(getACstate());
  temp += String(getSensorData());
  uint8_t data[temp.length() + 1];
  temp.getBytes(data, temp.length() + 1);
  rf95.send(data, temp.length());

  rf95.waitPacketSent();
  //
  //  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  //  uint8_t len = sizeof(buf);
  //
  //  if (rf95.waitAvailableTimeout(3000))
  //  {
  //    if (rf95.recv(buf, &len))
  //    {
  //      Serial.print("got reply: ");
  //      Serial.println((char*)buf);
  //      Serial.print("RSSI: ");
  //      Serial.println(rf95.lastRssi(), DEC);
  //    }
  //  }
  //  else
  //  {
  //    Serial.println("No ACK");
  //  }
}
int getSensorData() {
  int sensorValue = analogRead(A0);
  return sensorValue;
}
int getACstate() {
  int acState = digitalRead(ac);
  return acState;
}

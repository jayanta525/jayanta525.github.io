#define BAUDRATE 115200
#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <Process.h>
#include <String.h>
RH_RF95 rf95;
unsigned long previousMillis = 0;
const long interval = 2000;
float frequency = 868.0;
String old;
String IP = "10.130.1.231";
char client;
void setup() {
  Bridge.begin(BAUDRATE);
  Console.begin();
  if (!rf95.init())
    Console.println("init failed");
  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSpreadingFactor(7);
  rf95.setSignalBandwidth(125000);
  rf95.setCodingRate4(5);
  rf95.setSyncWord(0x34);
}
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    int command = checkforCommand() - 48;
    String neww = String(command);
    neww += String(client);
    if (old != neww)
      sendCommand(command);
  }
  receiveUpdates();
}
void sendCommand(int relaystate) {
  old = String(relaystate);
  old += String(client);
  String temp = String(client);
  temp += String(relaystate);
  uint8_t data[temp.length() + 1];
  temp.getBytes(data, temp.length() + 1);
  rf95.send(data, sizeof(data));
  rf95.waitPacketSent();
  Console.println("Sent a command");
}
void receiveUpdates() {
  if (rf95.available())
  {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      String sensor = "";
      for (int i = 2; i < len; i++) {
        sensor += buf[i] - 48;
      }
      uploadData(buf[0] - 48, buf[1] - 48, sensor);
    }
  }
}
void uploadData(int client, int relay, String sensor) {
  Process p;
  String URL = "http://";
  URL += IP;
  URL += "/receive.php?client=";
  URL += String(client);
  URL += "&access=";
  URL += String(relay);
  URL += "&sensor=";
  URL += String(sensor);
  p.begin("curl");
  p.addParameter(URL);
  p.run();
}
char checkforCommand() {
  char command[2] = {0};
  int count = 0;
  String URL = "http://";
  URL += IP;
  URL += "/new/";
  Process p;
  p.begin("curl");
  p.addParameter(URL);
  p.run();
  while (p.available() > 0){
    command[count] = p.read();
    count++;
  }
  client = command[0];
  return command[1];
}

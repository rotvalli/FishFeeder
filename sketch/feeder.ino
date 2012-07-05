/*!
* FishFeeder
* v1.0.0 - 2012-04-13
* Copyright (c) 2012 Otto Salminen; Licensed under GPL Version 2 %>
*/

#include <Ethernet.h>
#include <Servo.h>
#include <SPI.h>

const unsigned long DAY_IN_SECONDS = 86400;

Servo feedServo;

//Note: configure these to map your environment
byte mac[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
byte ip[] = { 0, 0, 0, 0 };
IPAddress timerServer(0,0,0,0);

EthernetClient client;

int servoPosition = 0;

String webDataCache;

unsigned long feedTime = 0;
unsigned long offsetTime = 0;
unsigned long clockTime = 0;
unsigned long timerSyncInterval = 1800; // 30 min
unsigned long previousMillis = 0; // Will store last time timer was synced

void setup()
{
  feedServo.attach(9);
  delay(15);
  feedServo.write(0);

  Serial.begin(9600);

  // Start Ethernet and sync timer
  Ethernet.begin(mac, ip);
  syncTimer();
}

void loop()
{

  //Sync clock and timer setting
  if(millis() - previousMillis > timerSyncInterval * 1000) {
    previousMillis = millis();
    syncTimer();
  }

  //Do feed
  //TO DO: Handle timing rollover http://arduino.cc/playground/Code/TimingRollover
  if(feedTime > 0 && clockTime * 1000 + (millis() - offsetTime) > feedTime * 1000){

    //Next feed after 24h
    feedTime += DAY_IN_SECONDS;

    doFeed();
  }

  if (client.available()) {
    char c = client.read();
    Serial.print(c);

    webDataCache += c;
  }

  // if the server's disconnected, stop the client:
  if (webDataCache != "" && !client.connected()) {
    setTimer();
    Serial.println();
    Serial.println("Disconnecting...");
    client.stop();
  }
}

void syncTimer(){
  if (client.connect(timerServer, 80)) {
    Serial.println("Connected");
    client.println("GET /timer.php HTTP/1.0");
    client.println();
    readPastHeader(&client);
  }
  else {
    Serial.println("Connection failed");
  }
}

void doFeed()
{
  Serial.println("Feeding...");
  for(servoPosition = 0; servoPosition < 180; servoPosition += 1)
  {
    feedServo.write(servoPosition);
    delay(15);
  }
  for(servoPosition = 180; servoPosition>=1; servoPosition-=1)
  {
    feedServo.write(servoPosition);
    delay(15);
  }
}

long stringToLong(String value)
{
  char arr[12];
  value.toCharArray(arr, sizeof(arr));
  return atol(arr);
}

void setTimer(){

  int splitIndex = 0;
  for(int i=0;i<webDataCache.length();i++){
    if(String(webDataCache[i]) == "\n"){
      break;
    }
    splitIndex++;
  }

  clockTime = stringToLong(webDataCache.substring(0, splitIndex));
  feedTime = stringToLong(webDataCache.substring(splitIndex+1));

  offsetTime = millis();

  if(feedTime < clockTime){
    feedTime += DAY_IN_SECONDS;
  }

  webDataCache = "";
}

bool readPastHeader(Client *pClient)
{
  bool bIsBlank = true;
  while(true)
  {
    if (pClient->available())
    {
      char c = pClient->read();
      if(c=='\r' && bIsBlank)
      {
        // throw away the /n
        c = pClient->read();
        return true;
      }
      if(c=='\n')
        bIsBlank = true;
      else if(c!='\r')
        bIsBlank = false;
    }
  }
}

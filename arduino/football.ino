#include <SoftwareSerial.h>
#include <string.h>
#include "screen.h"
#include "melody.h"
#include "ESP8266.h"
#define SSID        "HYS OFFICE WC"
#define PASSWORD    "380677117432"
#define HOST_NAME   "192.168.0.57"
#define HOST_PORT   (3000)
#define BEEPER 8 
#define NBR_MTX 2 
#define SENSOR_PIN_RED A1
#define SENSOR_PIN_BLUE A2
/*
 * RX is digital pin 11 (connect to TX of ESP8266)
 * TX is digital pin 10 (connect to RX of ESP8266)
 */
SoftwareSerial mySerial(11, 10);
ESP8266 wifi(mySerial);
int redScore;
int blueScore;
long previousMillis;

void setup()
{
  Serial.begin(9600);
  setMatrixIntensity(15);
  pinMode(BEEPER, OUTPUT);  // beeper output
  footballInitialize();
}

boolean setup_wifi(){
  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  return (wifi.setOprToStation() &&
    wifi.joinAP(SSID, PASSWORD) &&
    wifi.disableMUX() && 
    wifi.createTCP(HOST_NAME, HOST_PORT));
}

void send_post(char* url, char* contentType, char* content){
  char request[100] = "POST /";
  strcat(request, url);
  strcat(request, " HTTP/1.1\r\nContent-Type:");
  strcat(request, contentType);
  strcat(request, "\r\nContent-Length:");
  int n = strlen(content);
  char b[2];
  String str;
  str=String(n);
  str.toCharArray(b,2);
  strcat(request, b);
  strcat(request, "\r\n\r\n");
  strcat(request, content);
  strcat(request, "\r\n\r\n");
  Serial.println(request);
  wifi.send((const uint8_t*)request, strlen(request));
}

void send_ready(){
  Serial.println("ready");
  char *request = "GET /ready HTTP/1.1\r\n\r\n";
  Serial.println(request);
  wifi.send((const uint8_t*)request, strlen(request));
}

void send_playerId(char *id){
  char request[80] = "GET /player?id=";
  strncat(request, id, 12);
  strcat(request, " HTTP/1.1\r\n\r\n");
  Serial.println(request);
  wifi.send((const uint8_t*)request, strlen(request));
}

void send_red(){
  char *request = "GET /red HTTP/1.1\r\n\r\n";
  Serial.println(request);
  wifi.send((const uint8_t*)request, strlen(request));
}

void send_blue(){
  char *request = "GET /blue HTTP/1.1\r\n\r\n";
  Serial.println(request);
  wifi.send((const uint8_t*)request, strlen(request));
}


void loop()
{ 
  if((redScore > 9) || (blueScore > 9)){
    DisplayResult();    
    footballInitialize();
  } 
  else 
    ListenSensors();
}

void footballInitialize(){
  redScore = 0;
  blueScore = 0;
  while(!setup_wifi()){
    printStringWithShift("Connecting...    ", 30);
  }
  clearScreen();
  send_ready();
  delay(2000);
  get_players();
  DisplayScore();
}

void get_players(){
  int n = 0;
  boolean passed = false;
  char playerId[13] = "000000000000";
  char prevPlayerId[13] = "000000000000";
  while (n < 4) {
    char resultStr[16];
    Serial.flush();
    while(Serial.available() > 0) {
      Serial.read();
    }
    if(n == 0)
    {
      printStringWithShift("Red defender    ", 30);
      printString("R D", 0);
    }
    else if((n == 1)&&(passed))
    {
      printStringWithShift("Blue defender   ", 30);
      printString("B D", 0);
    }
    else if((n == 2)&&(passed))
    {
      printStringWithShift("Red forward    ", 30);
      printString("R F", 0);
    }
    else if((n == 3)&&(passed))
    {
      printStringWithShift("Blue forward    ", 30);
      printString("B F", 0);
    }

    while (Serial.available() == 0){
      delay(10);
    }
    Serial.readBytesUntil((char)3, resultStr, 13);
    memcpy(playerId, &resultStr[1], 12);
    Serial.println(playerId);
    Serial.print("prev: ");
    Serial.println(prevPlayerId);
    Serial.print("n: ");
    Serial.println(n);
    passed = (strcmp(playerId, prevPlayerId) != 0) || (n == 0);
    if(passed)
    {
      clearScreen();
      tone(8, 2000, 50);
      delay(50);
      n++;
      memcpy(prevPlayerId, playerId, 12);
      Serial.println(playerId);
      send_playerId(playerId);
    }
  }
}

void DisplayScore(){
  Serial.print(redScore);
  Serial.print(":");
  Serial.println(blueScore);
  printScore(redScore, blueScore);
}

void DisplayResult(){
  if (blueScore > redScore) {
    Serial.println("red win");
    printStringWithShift("Red win!!!    ", 30);  
  }
  else{
    Serial.println("blue win");
    printStringWithShift("Blue win!!!    ", 30);
  }
  playMelody();
}

void ListenSensors(){
  int redSensorValue = analogRead(SENSOR_PIN_RED); 
  int blueSensorValue = analogRead(SENSOR_PIN_BLUE); 
  //Serial.print(redSensorValue);    
  //Serial.print("   "); 
  //Serial.println(blueSensorValue);   
  if(redSensorValue > 900) {
    redScore ++;
    if (redScore < 10)
      DisplayScore();
    else
      clearScreen();
    send_red();
    delay(100);
  }  
  if(blueSensorValue > 900) {
    blueScore ++;
    if (blueScore < 10)
      DisplayScore();
    else
      clearScreen();
    send_blue();
    delay(100);
  }  
}













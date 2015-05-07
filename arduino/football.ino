#include <SoftwareSerial.h>
#include <string.h>
#include "ESP8266.h"
#include "LedControlMS.h"
/*
 * RX is digital pin 11 (connect to TX of ESP8266)
 * TX is digital pin 10 (connect to RX of ESP8266)
 */
SoftwareSerial mySerial(11, 10);
ESP8266 wifi(mySerial);
#define SSID        "HYS OFFICE WC"
#define PASSWORD    "380677117432"
#define HOST_NAME   "192.168.0.46"
#define HOST_PORT   (8888)
#define BEEPER 8 
#define NBR_MTX 2 
/*
 pin 7 is connected to the DataIn 
 pin 6 is connected to the CLK 
 pin 5 is connected to LOAD 
 */
LedControl lc = LedControl(7, 6, 5, NBR_MTX);

int redScore;
int blueScore;

void setup()
{
  setup_ledmatrix();
  redScore = 0;
  blueScore = 0;
  pinMode(BEEPER, OUTPUT);  // beeper output
  Serial.begin(9600);  // start serial to PC 
  if(setup_wifi())  
    Serial.println("ready");
  send_ready();
  getPlayers();
}

void setup_ledmatrix(){
  for (int i=0; i< NBR_MTX; i++){
    //lc.shutdown(i,false);
    /* Set the brightness to a low value */
    lc.setIntensity(i, 10);
    /* and clear the display */
    lc.clearDisplay(i);
  }
  delay(100);
  lc.clearAll();
}

void show_score_on_ledmatrix(int red, int blue)
{
  if(red > 10)
    red = 10;
  if(blue > 10)
    blue = 10;	
  lc.displayDigit(0, lc.getDigitArrayPosition(red,true));
  lc.displayDigit(1, lc.getDigitArrayPosition(blue,false));
}

void show_result_on_ledmatrix(bool redWins)
{
}

boolean setup_wifi(){
  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());
  return (wifi.setOprToStation() &&
    wifi.joinAP(SSID, PASSWORD) &&
    wifi.disableMUX() && 
    wifi.createTCP(HOST_NAME, HOST_PORT));
}

void send_ready(){
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
    setup();
  } 
  else 
    ListenSensors();
}

void getPlayers(){
  int n = 0;
  char playerId[16];
  char prevPlayerId[16] = "000000000000";
  while (n < 4) {
    char resultStr[16];
    Serial.flush();
    while(Serial.available() > 0) {
      Serial.read();
    }
    while (Serial.available() == 0){
      delay(10);
    }
    Serial.readBytesUntil((char)3, resultStr, 14);
    memcpy(playerId, &resultStr[1], 12);
    //Serial.println(playerId);
    //Serial.print("prev: ");
    //Serial.println(prevPlayerId);
    //Serial.print("n: ");
    //Serial.println(n);
    if((strcmp(playerId, prevPlayerId) != 0) || (n == 0))
    {
      n++;
      memcpy(prevPlayerId, playerId, 12);
      Serial.println(playerId);
      send_playerId(playerId);
      tone(BEEPER, 1000, 50);
    }
  }
}

void DisplayScore(){
  Serial.print(redScore);
  Serial.print(":");
  Serial.println(blueScore);
  show_score_on_ledmatrix(redScore, blueScore);
}

void DisplayResult(){
  if (blueScore > redScore) 
    Serial.println("red win");
  else
    Serial.println("blue win");
}

void ListenSensors(){
  int sensor1 = analogRead(A1); 
  int sensor2 = analogRead(A5); 
  if(sensor1 < 300) {
    redScore ++;
    DisplayScore();
    send_red();
    delay(100);
  }  
  if(sensor2 < 300) {
    blueScore ++;
    DisplayScore();
    send_blue();
    delay(100);
  }  
}

#include <SoftwareSerial.h>
SoftwareSerial RFID(2, 3); // RX and TX

byte playerID[16];
int redScore;
int blueScore;

void setup()
{
  redScore = 0;
  blueScore = 0;
  pinMode(8, OUTPUT);  // beeper output
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
  Serial.println("ready");
  RFID.flush();
  getPlayers();
}

void loop()
{ 
  if((redScore > 9) || (blueScore > 9)){
    DisplayResult();    
    setup();
  } else 
   ListenSensors();
}

void getPlayers(){
  int n = 0;
  while (n < 4){
    int i = 0;
    byte b = 0;
    boolean isEqual = true;
    while (RFID.available() > 0)
    {
      b = RFID.read();
      isEqual &= (playerID[i] == b);
      playerID[i++] = b;
      if ((RFID.available() == 0) && (!isEqual)) {
        tone(8, 2000, 50);
        for(int z = 0; z < 16; z++)
          Serial.print(playerID[z]);
        Serial.println();
        n++;
      }
    }
  }
}

void DisplayScore(){
  Serial.print(redScore);
  Serial.print(":");
  Serial.println(blueScore);
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
    delay(100);
  }  
  if(sensor2 < 300) {
    blueScore ++;
    DisplayScore();
    delay(100);
  }  
}

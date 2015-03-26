#include <SoftwareSerial.h>
SoftwareSerial RFID(2, 3); // RX and TX

byte playerID[16];

void setup()
{
  pinMode(8, OUTPUT);  // beeper output
  RFID.begin(9600);    // start serial to RFID reader
  Serial.begin(9600);  // start serial to PC 
  Serial.println("ready");
  getPlayers();
}

void loop()
{
 
}

void beep(int freq, float duration)
{
  int t = (int)(1 / 2 / freq * 1000000);
  int N = (int)(duration * freq);
  for(int j = 0; j < N; j++){
    digitalWrite(8, HIGH);   // turn the LED on (HIGH is the voltage level)
    delayMicroseconds(t);               // wait for a second
    digitalWrite(8, LOW);    // turn the LED off by making the voltage LOW
    delayMicroseconds(t);               // wait for a second
  }
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
        beep(1000, 0.1);
        for(int z = 0; z < 16; z++)
          Serial.print(playerID[z]);
        Serial.println();
        n++;
      }
    }
  }
}

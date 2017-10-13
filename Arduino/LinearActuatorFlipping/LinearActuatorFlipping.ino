#include <Servo.h> 
  
Servo myServo;
#define PIN_SERVO (5)
//FUNCTIONS
void SetStrokePerc(float strokePercentage)
{
  if ( strokePercentage >= 1.0 && strokePercentage <= 99.0 )
  {
    int usec = 1000 + strokePercentage * ( 2000 - 1000 ) / 100.0 ;
    myServo.writeMicroseconds( usec );
  }
}
void SetStrokeMM(int strokeReq,int strokeMax)
{
  SetStrokePerc( ((float)strokeReq) / strokeMax );
}
 
//variables
char Back                      = 'b';
char Front                     = 'f';

char Message;   
char UserMessage;    

void setup() 
{ 
  myServo.attach(PIN_SERVO);
  Serial3.begin(115200); // serialport3 to start communication with box
  Serial.begin(115200);
} 
  
 
void loop() 
{ 
  int d = 10;
  int delayMS = 1500;
  int i = 0;
  if (Serial3.available()||Serial.available()) {
      Message = Serial3.read();
      UserMessage = Serial.read();
    if (Message == Front || UserMessage == Front) {
    SetStrokePerc(95);
    delay(delayMS);
    }
    else if (Message == Back || UserMessage == Back) {
    SetStrokePerc(2);
    delay(delayMS);
    }
}
}

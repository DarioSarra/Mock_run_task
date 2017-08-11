/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
/* for continuous rotation servomotor write(n) doesn't indicate the
 *  position but the velocity of the rotation: 
 *  90 doesn't move
 *  0 is max speed clockwise
 *  180 is max speed counterclockwise
 */
int   InOut = 0;
int   Direction[]              = {0,180};
int   Break[]                  = {180,0};
int   Time[]                   = {2430,2500};

void setup() {
  Serial.begin(115200); // this to use the serialport to begin
  while (Serial.read() != 115) { //when 's' is pressed continue
  }
}

void loop() {

  myservo.attach(5);  // attaches the servo on pin 5 to the servo object
  Serial.println(String("begin"));
  Serial.println(String(Direction[InOut]));
  myservo.write(Direction[InOut]);
  Serial.println(String(InOut));
  delay(Time[InOut]);
  myservo.write(Break[InOut]);
  delay(30);
  myservo.detach();
  delay(2000);
  InOut = 1 - InOut;
}


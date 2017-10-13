 // Lets the water flow right then  left

int valvL = 7;
int valvR = 4;

//int aoPIN = 44;
int TimeDelay = 10000;

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(valvL, OUTPUT);
  pinMode(valvR, OUTPUT);
  //pinMode(aoPIN, OUTPUT);
  //digitalWrite(aoPIN, LOW);

  digitalWrite(valvR, HIGH);
  delay(TimeDelay);
  digitalWrite(valvR, LOW);
  digitalWrite(valvL, HIGH);
  delay(TimeDelay);
  digitalWrite(valvL, LOW); 
}

// the loop function runs over and over again forever
void loop() {

}

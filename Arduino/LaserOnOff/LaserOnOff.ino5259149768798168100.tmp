

long int stimdur = 10000; //milliseconds duration of one stim bout
long int pause = 2000;
int PulseWidth = 10; //in ms
int Serialinput = -1;
int ledHouse = 47;
int aoPIN = 46;

void setup() {
  // put your setup code here, to run once:

  pinMode(ledHouse, OUTPUT);
  pinMode(aoPIN, OUTPUT);
  digitalWrite(ledHouse, HIGH);
  digitalWrite(aoPIN, LOW);
  Serial.begin(115200);
  Serial.println('\n');

}

void loop() {
  // put your main code here, to run repeatedly:

  while (Serialinput == -1) {
     Serialinput = Serial.read();
  }
  digitalWrite(ledHouse, LOW);
  digitalWrite(aoPIN, HIGH);
  delay(stimdur);
  
  digitalWrite(ledHouse, HIGH);
  digitalWrite(aoPIN, LOW);
  delay(pause);

  //Serialinput = -1;
}






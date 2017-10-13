// DARIOOOO
/*//////////////       Flipping Task       ////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  /////////// February 2016 Matthijs oude Lohuis //////////////////////////*/

//////////////                I/O Mapping

//INPUTS
int pkL = 5;
int pkR = 11;
//OUTPUTS
int ledL = 6;
int ledR = 12;
int valvL = 7;
int valvR = 4;

//////////////                Task parameters

unsigned long durRewDelivL = 39; //Calibrate valves for x ul 30000 is good size to flush!
unsigned long durRewDelivR = 41; //Calibrate valves for x ul

int RewardCount = 1;
int Amountofrewards = 150;


/* -------------------------------------------------------------------
   ----------------------------SETUP-----------------------------------*/

void setup() {

  //INPUTS
  pinMode(pkL, INPUT);
  pinMode(pkR, INPUT);

  //OUTPUTS
  pinMode(ledL, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(valvL, OUTPUT);
  pinMode(valvR, OUTPUT);

  //SET DEFAULT
  digitalWrite(ledL, HIGH); //HIGH is LED off, LOW is LED on
  digitalWrite(ledR, HIGH); //HIGH is LED off, LOW is LED on
  digitalWrite(valvL, LOW);
  digitalWrite(valvR, LOW);

  // Start writing to txt on computer via serial
  Serial.begin(115200);
  //while (Serial.read() != 115) { //when 's' is pressed (or sent via python) continue
  //}
  Serial.println(String(98) + '\t' + String(millis())); // 98 Starting Code

}


/* -------------------------------------------------------------------
   ----------------------------LOOP-----------------------------------*/

void loop() {

  while (RewardCount <= Amountofrewards) {
    delay(60); //Delay between waterdeliveries

    digitalWrite(ledL, LOW); // turn led on
    digitalWrite(valvL, HIGH);
    delay(durRewDelivL);
    digitalWrite(valvL, LOW);
    digitalWrite(ledL, HIGH); // turn led off

    delay(60); //delay in between two sides

    digitalWrite(ledR, LOW); // turn led on
    digitalWrite(valvR, HIGH);
    delay(durRewDelivR);
    digitalWrite(valvR, LOW);
    digitalWrite(ledR, HIGH); // turn led off

    Serial.println(String(RewardCount) + '\t' + String(millis())); // 98 Starting Code
    RewardCount++;
   // break;
  }

}

/*//////////////       Flipping Foraging Fiber photo!!       //////////////////////////////
  ///////////     2016 Pietro Vertechi     //////////////////////////*/

//////////////                I/O Mapping

// 0 is right, 1 is left

// Find smart way of inputting GamVec and delta
// Take care of this sound + light nonsense!!
// 70 = duringpoking off
// 71 = duringpoking on

//INPUTS
int pk[2] = {11,5};

//OUTPUTS
//int syncLED = 2;
//int spkrL = 3;
int spkR = 3;
int led[] = {12, 6};
int valv[] = {4, 7};
bool toggled = false;
int ledHouse[] = {10,2}; //10 is central poke (used as syncing LED), 2 is syncing LED(National instrument)
int aoPIN = 44;

//////////////                Set Task parameters
//General settings:
unsigned long   MinPokeTime = 100;            //Time an individual poke has to last to count as a poke (to discard whisker detection, e.g.)

//Reward duration, calibrate here   (Vector of calibrated values for integers:)
int   durRewBox2[] = {62, 46}; // 0 is right 1 is left
int   durRewBox3[] = {38, 38}; // 0 is right 1 is left
int   durRewBox4[] = {45, 44}; // 0 is right 1 is left
int   durRew[2];

//////////////              Internal variables for Task
//Get task parameters from python later on:
int   Choice                 = -1;
int   ProbVec[]              = {80,20};
int   GamVec[]               = {100,100};
int   delta;
int   Box                    = -1;
int   SessionStim            = -1;
int   Counter                = 1; //counter of the streaks updated in the void loop case 2
int   Blocklength;


//For Task:
int TimeAfterPoke = 0;  // How long to keep sound after Poke out
unsigned long TimeLastPkIn;
unsigned long TimeLastPokeOut = 0;
int valuePk[] = {0, 0};
int state = 1;
int Protocollo;
int SideProtocollo;
unsigned long Freq = 8500;
int Side = -1;         //To create signal at every pokesideswitch
int PotSide = -1;
int HighSide;
boolean Reward = false;
boolean PokedIn = false;
boolean error = false;
unsigned long StopRewardTargetTime;
unsigned long RSeed;


// for stim

unsigned long   StimFreq               = 25;       //Frequency of masking LED
unsigned long   OnTime                 = 10;       //PulseWidth house LED

// for sound
boolean DuringPoking = false;
boolean SoundCurrOn = false;
unsigned long PokingOnset = 0;
/* -------------------------------------------------------------------
   ----------------------------SETUP-----------------------------------*/

void setup() {

  //INPUTS
  pinMode(pk[0], INPUT);
  pinMode(pk[1], INPUT);

  //OUTPUTS
  //  pinMode(syncLED, OUTPUT);
  //  pinMode(spkrL, OUTPUT);
  pinMode(spkR, OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[0], OUTPUT);
  pinMode(valv[1], OUTPUT);
  pinMode(valv[0], OUTPUT);
  pinMode(ledHouse[0], OUTPUT);
  pinMode(ledHouse[1], OUTPUT);
  pinMode(aoPIN, OUTPUT);

  //SET DEFAULT
  //  digitalWrite(syncLED, HIGH);  //HIGH is LED off, LOW is LED on
  digitalWrite(led[1], HIGH);     //HIGH is LED off, LOW is LED on
  digitalWrite(led[0], HIGH);     //HIGH is LED off, LOW is LED on
  digitalWrite(valv[1], LOW);
  digitalWrite(valv[0], LOW);
  digitalWrite(ledHouse[1], LOW);
  digitalWrite(ledHouse[0], LOW);

  // Start writing to txt on computer via serial
  Serial.begin(115200);
  Serial.println(String(-98) + '\t' + String(millis())); // 98 Starting Code

  // Define starting LhighRlow or RhighLlow randomly
  //pinMode(0, INPUT);
  RSeed = analogRead(0);

  randomSeed(RSeed); //generate different seed numbers each time the sketch runs by analog noise
  //Serial.println(String(-RSeed) + '\t' + String(millis()));

  Protocollo = random(0,2);
  HighSide = random(0,2);

  //Get information from python
  while (Choice == -1) { Choice = Serial.read();}
  //Serial.println(String(-10*Choice) + '\t' + String(millis()));
  while (Box == -1) { Box = Serial.read();}
  //Serial.println(String(-10*Box) + '\t' + String(millis()));
  while (SessionStim == -1) {SessionStim = Serial.read();}
  //Serial.println(String(-10*SessionStim-10) + '\t' + String(millis()));

  if (Box == 2) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox2[i];} }
  if (Box == 3) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox3[i];} }
  if (Box == 4) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox4[i];} }

  switch(Choice) {

      case 1:
        Protocollo = 0;  //check with Pietro if it should be removed or kept=0
        delta = 0;
        Blocklength = random(35,46);
        break;

      case 2:
        Protocollo = 1;
        delta = 0;
        break;

      case 3:
        delta = 5;
        break;

      case 4:
        delta = 50;
        break;

      case 5:
        Protocollo = 0;
        delta = 0;
        ProbVec[0]=90;
        break;
  }


  Serial.println(String(ProbVec[0]-100) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(ProbVec[1]-100) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(GamVec[0]-100) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(GamVec[1]-100) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(delta-100) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(-666) + '\t' + String(millis())); // Signal that all is well
}

/* -------------------------------------------------------------------
   ----------------------------LOOP-----------------------------------*/

  // variables = protocollo, valid poke, state

void loop() {
  updatesoundled();
  task();

}
  /* -------------------------------------------------------------------
   ----------------------------TASK-----------------------------------*/

  // variables = protocollo, valid poke, state

void task() {

  // Read pokes and check whether LED and speaker need to stop or if the animal has poked out

  for (int i = 0; i < 2; i++) { valuePk[i] = digitalRead(pk[i]);}
  // Detect Poke Out

  if (!valuePk[1] && !valuePk[0] && PokedIn) {
    TimeLastPokeOut = millis();
    Serial.println(String(-1) + '\t' + String(millis())); // Time of Poke Out
    PokedIn = false;
  }

  // check if you should turn off sound and LED
  if ((millis() > TimeLastPokeOut + TimeAfterPoke) && DuringPoking && !PokedIn ) {
    DuringPoking = false;
    Serial.println(String(-70) + '\t' + String(millis()));
  }

  switch(state) {

    case 1: //Waiting for poke
      for (int x = 0; x < 2; x++) {
        if (valuePk[x] && !PokedIn) { //Left poke
          Reward = false;
          PotSide = x;
          state = 2;
          TimeLastPkIn = millis();
          break;
        }
      }
      break;

    case 2: // Checking validity, then deciding reward, gamma and protocol transition
      if (!valuePk[PotSide]) { // premature poke out!
        state = 1;
        PokedIn = false;
      }
      if ((millis() - TimeLastPkIn) > MinPokeTime) { // valid poke
        PokedIn = true;
        if (PotSide != Side) {
          Side = PotSide;
          error = (Side != HighSide);
          if (!error) {
            if (random(1,101) <= delta) {
              Protocollo = 1-Protocollo;
            }
          Counter = 1+Counter;//this keeps the count of the valid streaks
          if(Counter>=Blocklength) { //non poisson transition, if streak number is major than rand streak sorted between 35 and 45 Protocol changes
              Protocollo = 1-Protocollo;
              Counter = 1;
              Blocklength = random(35,46); //Blocklength must be re-sorted to avoid same length blocks
            }
          }
        }
        if (!DuringPoking) {
            DuringPoking = true;
            PokingOnset = millis();
            Serial.println(String(-71) + '\t' + String(millis()));
        }
        SideProtocollo = Protocollo*(1-Side)+Side*(1-Protocollo);
        if (HighSide == Side) {
          Reward = (random(1,101) <= ProbVec[SideProtocollo]);
        }
        if (Reward) {
          digitalWrite(valv[Side], HIGH);
          StopRewardTargetTime = millis() + durRew[Side];
          state = 3;
        }
        else {
          state = 1;
        }
        Serial.println(String(Reward+2*Side+4*HighSide+8*SideProtocollo)
        + '\t' + String(TimeLastPkIn));
        if ((HighSide == Side) && (random(1,101) <= GamVec[SideProtocollo])) {
          HighSide = 1- HighSide;
        }
      }
    break;

    case 3:
      if (millis() > StopRewardTargetTime) {
        digitalWrite(valv[0], LOW);
        digitalWrite(valv[1], LOW);
        state = 1;
      }
      break;
  }
}

/* -------------------------------------------------------------------
   --------------------  UPDATE HOUSE LED ---------------------------*/

void updatesoundled() {
  //See if sound and HouseLED needs to be turned on or off:
  if (DuringPoking && (Side == HighSide)) {
      tone(spkR, Freq);
  }
  else {
      noTone(spkR);
  }
  if (DuringPoking && !toggled) {
      for (int i = 0; i < 2; i++) { digitalWrite(ledHouse[i], HIGH);}
      toggled = true;
  }
  if (!DuringPoking && toggled) {
      for (int i = 0; i < 2; i++) { digitalWrite(ledHouse[i], LOW);}
      toggled = false;
  }
}

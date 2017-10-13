/*//////////////       Flipping Foraging       //////////////////////////////
  ///////////     2016 Pietro Vertechi     //////////////////////////*/

//////////////                I/O Mapping

// 0 is right, 1 is left

// Find smart way of inputting GamVec and delta
// Take care of this sound + light nonsense!!
// 70 = duringpoking off
// 71 = duringpoking on

//INPUTS
int pk[2] = {11,5}; //IRs right(0) and left(1)
#define Poke_IR_Right A2
#define Poke_IR_Left A8
#define read_IR_Right digitalRead(Poke_IR_Right)
#define read_IR_Left digitalRead(Poke_IR_Left)

//OUTPUTS
int syncLED = 47;       // Square wave from 100ms after poke in to poke out, PORT: SPKR3
int spkR = 3;          // speaker for valid pokes PORT: SPKR1
int led[] = {12, 6};    // LEDs right(0) and left(1) pokes PORT: POKE_1, POKE_3
int valv[] = {4, 7};    // valves right(0) and left(1) pokesPORT: POKE_1, POKE_3
int ledHouse = 10;      // masking light PORT: POKE2 (starts 100ms after poke in, ends TimeAfterPoke after poke out, with the frequency of the stimulation)
int aoPIN = 46;         // laser stimulation PORT: LZR 
//////////////                Set Task parameters
//General settings:
unsigned long   MinPokeTime = 100;            //Time an individual poke has to last to count as a poke (to discard whisker detection, e.g.)

//Reward duration, calibrate here   (Vector of calibrated values for integers:)
int   durRewBox2[] = {52, 45}; // 0 is right 1 is left
int   durRewBox3[] = {39, 41}; // 0 is right 1 is left
int   durRewBox4[] = {42, 43}; // 0 is right 1 is left
int   durRew[2];

//////////////              Internal variables for Task
//Get task parameters from python later on:
int   Choice                 = -1;
int   ProbVec[]              = {45,90};
int   GamVec[]               = {25,50};
int   delta;
int   Box                    = -1;
int   SessionStim            = -1;
int   Counter                = 0;
int   BlockLength;
int   MinBlockLength         = 11;
int   MaxBlockLength         = 15;


//For Task:
int TimeAfterPoke = 500;  // How long to keep sound after Poke out
unsigned long TimeLastPkIn;
unsigned long TimeLastPokeOut = 0;
int valuePk[] = {0, 0};
int state = 1;
int Protocollo;
int Start = 0; //use to randomly set the wall in or out at the beginning of the session
unsigned long Freq = 8500;
int Stim = 0;
int Side = -1;         //To create signal at every pokesideswitch
int PotSide = -1;
int HighSide;
boolean Reward = false;
boolean PokedIn = false;
boolean error = false;
boolean WallOn = false;
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
  //pinMode(pk[0], INPUT);
  //pinMode(pk[1], INPUT);
  pinMode(Poke_IR_Right, INPUT);
  pinMode(Poke_IR_Left, INPUT);

  //OUTPUTS
  pinMode(syncLED, OUTPUT);
  //  pinMode(spkrL, OUTPUT);
  Serial3.begin(115200); //activate serial port 3 rx and tx
  pinMode(spkR, OUTPUT);
  pinMode(led[1], OUTPUT);
  pinMode(led[0], OUTPUT);
  pinMode(valv[1], OUTPUT);
  pinMode(valv[0], OUTPUT);
  pinMode(ledHouse, OUTPUT);
  pinMode(aoPIN, OUTPUT);
  //SET DEFAULT
  digitalWrite(syncLED, HIGH);  //HIGH is LED off, LOW is LED on
  digitalWrite(led[1], HIGH);     //HIGH is LED off, LOW is LED on
  digitalWrite(led[0], HIGH);     //HIGH is LED off, LOW is LED on
  digitalWrite(valv[1], LOW);
  digitalWrite(valv[0], LOW);
  digitalWrite(ledHouse, LOW);

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

  Stim = random(0,1+SessionStim); // initialize Stim variable

  if (Box == 2) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox2[i];} }
  if (Box == 3) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox3[i];} }
  if (Box == 4) { for (int i = 0; i < 2; i++) {durRew[i] = durRewBox4[i];} }

  switch(Choice) {

      case 1:
        Protocollo = 0;
        delta = 0;
        break;

      case 2:
        Protocollo = 1;
        delta = 0;
        break;

      case 3:
        delta = 5;
        break;

      case 4:
        Protocollo = 0;
        delta = 0;
        break;

      case 5:
        Protocollo = 0;
        delta = 0;
        ProbVec[0]=90;
        break;

      case 6:
        Protocollo = 0;
        delta = 0;
        Start = random(1,3);
        BlockLength = random(MinBlockLength,MaxBlockLength);
        if (Start == 1) {
          WallOn = false;
          Serial3.write('b');
        }
        else if (Start == 2){
          WallOn = true;
          Serial3.write('f');
        }
        break;

        case 7:
        Protocollo = 1;
        delta = 0;
        Start = random(1,3);
        BlockLength = random(MinBlockLength,MaxBlockLength);
        if (Start == 1) {
          WallOn = false;
          Serial3.write('b');
        }
        else if (Start == 2){
          WallOn = true;
          Serial3.write('f');
        }
        break;
  }


  Serial.println(String(ProbVec[0]) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(ProbVec[1]) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(GamVec[0]) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(GamVec[1]) + '\t' + String(millis())); // Signal that all is well
  delay(100);
  Serial.println(String(delta) + '\t' + String(millis())); // Signal that all is well
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

  // Read pokes and check whethe LED and speaker need to stop or if the animal has poked out

  // for (int i = 0; i < 2; i++) { valuePk[i] = digitalRead(pk[i]);}
  // Detect Poke Out
  valuePk[0] = read_IR_Right;
  valuePk[1] = read_IR_Left;

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
          Stim = random(0,1+SessionStim);
          if (!error) {
            if (random(1,101) <= delta) {
              Protocollo = 1-Protocollo;
            }
            Counter = Counter +1;
            if(Counter >= BlockLength){ //non poisson transition, if streak number is major than rand streak sorted between 35 and 45 Protocol changes
              if(WallOn){
                Serial3.write('b');
              }
              else if (!WallOn){
                Serial3.write('f');
              }
              Serial.println(String(-91) + '\t' + String(millis()));
              WallOn = !WallOn;
              Counter = 0;
              BlockLength = random(MinBlockLength,MaxBlockLength); //BlockLength must be re-sorted to avoid same length blocks
            }
          }
        }
        if (!DuringPoking) {
            DuringPoking = true;
            PokingOnset = millis();
            Serial.println(String(-71-10*Stim) + '\t' + String(millis()));
        }
        if (HighSide == Side) {
          Reward = (random(1,101) <= ProbVec[Protocollo]);
        }
        if (Reward) {
          digitalWrite(valv[Side], HIGH);
          StopRewardTargetTime = millis() + durRew[Side];
          state = 3;
        }
        else {
          state = 1;
        }
        Serial.println(String(Reward+2*Side+4*HighSide+8*Protocollo+16*Stim+32*WallOn)
        + '\t' + String(TimeLastPkIn));
        if ((HighSide == Side) && (random(1,101) <= GamVec[Protocollo])) {
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
  if (DuringPoking && !error) {
      tone(spkR, Freq);
  }
  else {
      noTone(spkR);
  }
  if (DuringPoking && PokedIn) {
    digitalWrite(syncLED, HIGH);
  }
  else {
    digitalWrite(syncLED, LOW);
  }
  if (DuringPoking && ((millis()-PokingOnset)*StimFreq % 1000 < OnTime*StimFreq)) {
      digitalWrite(ledHouse, HIGH);
      if (Stim == 1) {
        digitalWrite(aoPIN, HIGH);
      }
      else {
        digitalWrite(aoPIN, LOW);
      }
  }
  else {
      digitalWrite(ledHouse, LOW);
      digitalWrite(aoPIN, LOW);
  }
}

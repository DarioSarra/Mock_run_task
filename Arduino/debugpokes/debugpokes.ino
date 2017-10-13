//INPUTS
//int pk[2] = {11, 5}; //IRs right(0) and left(1)
#define Poke_IR_Right A2
#define Poke_IR_Left A8
#define read_IR_Right digitalRead(Poke_IR_Right)
#define read_IR_Left digitalRead(Poke_IR_Left)

//OUTPUTS
int syncLED = 47;       // Square wave from 100ms after poke in to poke out, PORT: SPKR3
int spkR = 3;           // speaker for valid pokes PORT: SPKR1
int led[] = {12, 6};    // LEDs right(0) and left(1) pokes PORT: POKE_1, POKE_3
int valv[] = {4, 7};    // valves right(0) and left(1) pokesPORT: POKE_1, POKE_3
int ledHouse = 10;      // masking light PORT: POKE3 (starts 100ms after poke in, ends TimeAfterPoke after poke out, with the frequency of the stimulation)
int aoPIN = 46;         // laser stimulation PORT: LZR
unsigned long Freq = 8500;
//States
int state = 1;
boolean PokedIn = false;
int PotSide = -1;
int Side = -1;
boolean DuringPoking = false;
int valuePk[] = {0, 0};
unsigned long PokingOnset = 0;
unsigned long TimeLastPkIn;
//////////////                Set Task parameters
//General settings:
unsigned long   MinPokeTime = 100;            //Time an individual poke has to last to count as a poke (to discard whisker detection, e.g.)
unsigned long StopRewardTargetTime;
//Reward duration, calibrate here   (Vector of calibrated values for integers:)
int   durRewBox2[] = {52, 45}; // 0 is right 1 is left
int   durRewBox3[] = {39, 41}; // 0 is right 1 is left
int   durRewBox4[] = {42, 43}; // 0 is right 1 is left
int   durRew[2];

/* -------------------------------------------------------------------
   ----------------------------SETUP-----------------------------------*/

void setup() {
  //INPUTS
  pinMode(Poke_IR_Right, INPUT);
  pinMode(Poke_IR_Left, INPUT);

  //OUTPUTS
  pinMode(spkR, OUTPUT);
  pinMode(valv[1], OUTPUT);
  pinMode(valv[0], OUTPUT);
  pinMode(ledHouse, OUTPUT);
}


void loop() {
  updatesoundled();
  task();
}

   /* -------------------------------------------------------------------
   ----------------------------TASK-----------------------------------*/

  // variables = protocollo, valid poke, state
void task() {
  // put your main code here, to run repeatedly:
  valuePk[0] = read_IR_Right;
  valuePk[1] = read_IR_Left;

  if (!valuePk[1] && !valuePk[0] && PokedIn) {
    PokedIn = false;
  }

  switch (state) {

    case 1: //Waiting for poke
      for (int x = 0; x < 2; x++) {
        if (valuePk[x] && !PokedIn) { //Left poke
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
        Side = PotSide;
        digitalWrite(valv[Side], HIGH);
          StopRewardTargetTime = millis() + durRew[Side];
           if (!DuringPoking) {
            DuringPoking = true;
            PokingOnset = millis();
          state = 3;
        }
      }
        break;
     case 3:
      if (millis() > StopRewardTargetTime) {
        digitalWrite(valv[0], LOW);
        digitalWrite(valv[1], LOW);
        PokedIn = false;
        state = 1;
      }
      break;
  }
  }

/* -------------------------------------------------------------------
   --------------------  UPDATE HOUSE LED ---------------------------*/
  void updatesoundled() {
  //See if sound and HouseLED needs to be turned on or off:
  if (DuringPoking) {
      tone(spkR, Freq);
      digitalWrite(syncLED, HIGH);
      digitalWrite(ledHouse, LOW);
  }
  else {
      noTone(spkR);
      digitalWrite(syncLED, LOW);
      digitalWrite(ledHouse, HIGH);
  }
  
  }


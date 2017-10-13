//INPUTS
int pk[2] = {11, 5}; //IRs right(0) and left(1)
#define Poke_IR_Right A2
#define Poke_IR_Left A8
#define read_IR_Right digitalRead(Poke_IR_Right)
#define read_IR_Left digitalRead(Poke_IR_Left)
int valuePk[] = {0, 0};
//OUTPUTS
int aoPIN = 46;
int syncLED = 47;

void setup() {
  pinMode(Poke_IR_Right, INPUT);
  pinMode(Poke_IR_Left, INPUT);
  pinMode(aoPIN, OUTPUT);
  pinMode(syncLED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  valuePk[0] = read_IR_Right;
  valuePk[1] = read_IR_Left;
  for (int x = 0; x < 2; x++) {
    if (valuePk[x]) {
      digitalWrite(aoPIN, HIGH);
      digitalWrite(syncLED, HIGH);
      delay(1000);
      digitalWrite(aoPIN, LOW);
      digitalWrite(syncLED, LOW);
    }
  }
}

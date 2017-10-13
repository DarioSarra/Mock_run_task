char Message;   
char UserMessage; 
void setup() {
  // put your setup code here, to run once:
  Serial3.begin(115200); //activate serial port 3 rx and tx
  Serial.begin(115200);
  
}

void loop() {
  int delayMS = 2500;
 Serial3.write('b');
 delay(delayMS);
  Serial3.write('f');
}

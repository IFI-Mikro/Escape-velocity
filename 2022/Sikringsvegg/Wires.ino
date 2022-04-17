int activeInputs[] = {2, 3, 4, 5, 6};
int numInputs = 5;
bool mistake = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);

  digitalWrite(10, HIGH);

  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  delay(1000);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  delay(100);
  for (int i = 0; i < numInputs; i++) {
    pinMode(activeInputs[i], INPUT);
  }
  bool notActive = false; // should be true...
  while (notActive) {
    notActive = false;
    for (int i = 0; i < numInputs; i++) {
      if (digitalRead(activeInputs[i]) != HIGH) {
        notActive = true;
        digitalWrite(11, LOW);
      }
    }
  }
  delay(100);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);



  //delay(3000);
  //activeLight();
}
void loop() {
  
  // put your main code here, to run repeatedly:
  int droppedInputs = 0;
  for (int i = 0; i < numInputs; i++) {
    if (digitalRead(activeInputs[i]) != HIGH) {
      delay(100);
      if (digitalRead(activeInputs[i]) != HIGH) {
        activeInputs[i] = activeInputs[(numInputs - 1)];
        droppedInputs++;
        mistake = true;
      }
    }
  }


  if (mistake) {

    activeLight();
    mistake = false;

  }
  numInputs -= droppedInputs;
  passiveLight();
  
}

void activeLight() {
  // NOE gikk galt!
  /* Real method:
      Using:
      taser >> 13
      light on >>> 12, 11, 10
  */
  digitalWrite(10, LOW); //taser on
  digitalWrite(12, HIGH);
  for (int i = 0; i < 4; i++) {
    digitalWrite(11, HIGH);
    delay(100 - 20 * i);
    digitalWrite(13, HIGH);
    delay(70 + 20 * i);
    if (i == 2) {
      digitalWrite(11, LOW);
      digitalWrite(10, HIGH); //taser off
    }
    delay(20 + 20 * i);
    digitalWrite(13, LOW);
    delay(100 - 20 * i);
  }
}

void passiveLight() {
  // make sure the light is on...
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
  digitalWrite(13, LOW);
  digitalWrite(10, HIGH); // and that the taser is off.
  delay(1);
}





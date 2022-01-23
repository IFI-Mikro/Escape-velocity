/*
  Denne koden er en del av prosjektet Escape Velocity,
  og mer spesifikt rørveggen, der målet er å løfte 3
  pingpongballer til riktig høyde. Dette gjøres ved å
  lage en riktig spenningsfall, som får en vifte til å
  snurre akkurat så fort at pingpongballen havner i
  riktig høyde.

  Skjult mellom spenningsdeleren de bygger og viften de bruker,
  ligger en arduino og kjører denne koden. Det er fordi
  undertegnede ikke klarte å få dette til å funke uten å
  brenne opp en hel haug med resistorer.

  Dette programmet leser inn en spenning på tre analoge pins,
  for så å oversette det til et spenningsnivå som passer
  viften og skrive ut dette på tre PMW pins.

  Komponenter brukt:
  3x Vifte
  3x 10K Ohm Resistor
  3x 1N4007 Diode (?)
  3x TIP120 Transistor

  Det bør finnes et arduino-skjold med disse loddet
  fast på et sted.

  Koden er skrevet av Hilmar Elverhøy og kan brukes av alle
  som ikke tror de kunne gjordt det bedre selv.
 */


//Her må man bruke pins som er merket med PMW
int digitalPins[] = {11,  9, 10};

int analogPins[] = {A0, A1, A2};


void setup() {
  pinMode(3, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(digitalPins[i],  OUTPUT);
   
  }
  Serial.begin(9600);
  digitalWrite(3, HIGH);
  for (int i = 0; i < 3; i++) {
    // Her leser vi inn spenningen på de analoge pinsene og
    // skriver den tilpassede verdien til viften
    int sensorValue = analogRead(analogPins[i]);
    int convertedValue = convert(sensorValue, i);
    analogWrite(digitalPins[i], convertedValue);
    Serial.print("innverdi: ");
    Serial.println(sensorValue);
    delay(5000);
    analogWrite(digitalPins[i], 0);
    delay(500);
  }

}
void(* resetFunc) (void) = 0;

void loop() {
  for (int i = 255; i > 40; i--){
    analogWrite(3, i);
    delay(10);
    if (analogRead(A3) >500){
      resetFunc();
    }
  }
}


int convert(int input, int fan) {
  int lowerBound = 100;   // Laveste den skal bli
  int upperBound = 240;   // Høyeste den skal bli
  
  
  int span = upperBound - lowerBound;
  int factor = 1024 / span;
  
  int value = input / factor + lowerBound;
  
  int correctValue;
  switch (fan){
    case 0:
      //560 analog verdi
      correctValue = 180;
    case 1:
      //294
      correctValue = 142;
    case 2:
      //420
      correctValue = 160;
    default:
      correctValue = 127;
  }
 
  if (abs(value - correctValue) < 5){
    value = correctValue;
  }
  return value;
}


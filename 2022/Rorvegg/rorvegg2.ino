/*
  Denne koden er en del av prosjektet Escape Velocity,
  og mer spesifikt rørveggen, der målet er å løfte 3
  pingpongballer til riktig høyde veg å lage en lukket krets.
   
  Skjult mellom kretsen de bygger og viften de bruker,
  ligger en arduino og kjører denne koden.
  Dette programmet leser inn en spenning på tre digitale pins,
  for så å oversette det til et spenningsnivå som passer
  viften og skrive ut dette på tre PMW pins. Dersom kretsen er lukket vil
  ballene gå til riktig høyde.
  Hvis kretsen ikke er lukket vil ballene gå til en tilfeldig høyde.
  
  Komponenter brukt:
  3x Vifte
  3x 10K Ohm Resistor
  3x 1N4007 Diode (?)
  3x TIP120 Transistor
  
  Det bør finnes et arduino-skjold med disse loddet
  fast på et sted.
  Koden er skrevet av Tobias Lømo med god hjelp av Hilmar sin kode fra 2018 og kan brukes av alle
  som ikke tror de kunne gjordt det bedre selv.
 */

 /*
  * fan1: 155
  * fan2: 125
  * fan3: 200
  */


//Her må man bruke pins som er merket med PMW
int digitalPins[] = {11,  9, 10};

//input pins
int inputPins[] = {4, 5, 6};
int wrongInputPin = 0;

//Riktige pwm verdier for viftene. Disse verdiene varierer litt
const int FAN1 = 155;
const int FAN2 = 125;
const int FAN3 = 200;

//Sett til true hvis man vil få ut informasjon på serial port.
const int DEBUG = true;


void setup() {
  /* Setter opp programmet.
   * De digitle inputene er koblet med en pullup. Dette gjør at de vil være aktiv høy.
   * Setup tester også lle viftene en rask gang for å skjekke om de funger
   */
   
  Serial.begin(9600);
  pinMode(3, OUTPUT);

  
  for (int i = 0; i < 3; i++) {
    pinMode(digitalPins[i],  OUTPUT);
    pinMode(inputPins[i], INPUT_PULLUP);
    
    Serial.print("Testing: ");
    Serial.println(i);
    analogWrite(digitalPins[i], 150);
  }

  pinMode(wrongInputPin, INPUT_PULLUP);

  delay(3000);
  for(int i = 0; i < 3; i++) {
    analogWrite(digitalPins[i], 0);
  }
  
  
  digitalWrite(3, HIGH);

  randomSeed(analogRead(A0));
  Serial.println("Ready");
}

void loop() {
  /*
   * Går igjennom og skjekker om knappen har blitt trykket
   */
  for (int i = 255; i > 40; i--){
    analogWrite(3, i);
    delay(10);
    if (analogRead(A3) > 500){
      check_results();
    }
  }
}


void check_results() {
  /*
   * Leser av resultatet på input og kaller på riktig funksjon
   */
  for(int i = 0; i < 3; i++) {
    int res = digitalRead(inputPins[i]);
    int wrongRes = digitalRead(wrongInputPin);
    Serial.print(i);
    Serial.print(": ");
    Serial.print(!res);
    Serial.print("  WrongPin: ");
    Serial.println(!wrongRes);

    if (wrongRes == LOW) {
      fan_random(i);
    } else if (res == LOW) {
      fan_correct(i);
    } else {
      fan_random(i);
    }
  }
  
  Serial.println("\n");
}

void fan_correct(int fan) {
  /*
   * Skriver riktig verdi til viften
   */
  int fanValue = 0;

  if(fan == 0) {
    fanValue = FAN1;
  } else if(fan == 1) {
    fanValue = FAN2;
  } else if(fan == 2) {
    fanValue = FAN3;
  } else {
    Serial.println("Hmmmm rart");
  }

  if(DEBUG) {
    Serial.print("Running fan ");
    Serial.print(fan);
    Serial.print(" on ");
    Serial.println(fanValue);
  }
  
  analogWrite(digitalPins[fan], fanValue);
  delay(5000);
  analogWrite(digitalPins[fan], 0);
  delay(250);
}

void fan_random(int fan) {
  /*
   * Skriver en tilfeldig verdi til viften.
   * Dersom den tilfeldige verdien er for nærme den riktige vil den prøve på nytt i et rekursvit kall
   */
  int fanList[] = {FAN1, FAN2, FAN3};
  long r = random(95, 210);

  if (abs(r - fanList[fan]) < 20) {
    fan_random(fan);
  } else {
    if(DEBUG) {
      Serial.print("Running fan ");
      Serial.print(fan);
      Serial.print(" on ");
      Serial.println(r);
    }
    
    analogWrite(digitalPins[fan], r);
    delay(5000);
    analogWrite(digitalPins[fan], 0);
    delay(250);
  } 
}

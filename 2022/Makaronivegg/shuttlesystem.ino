/*
 * Tobias Lømo 17.4.2022
 * 
 * Kode til shuttlesystemet også kjent som makaroniveggen.
 * Inspirasjon fra koden i 2018, men skrevet om ettersom vi ikke har den koden (spør hilmar).
 * En veldig enkel versjon av veggen, men siden den unsett er 
 * ganske vanskelig så er det sikkert gret.
 * 
 */
#include <Servo.h>

int led = 13;

int hold_button = 8;
int push_button = 9;

//Sett til true hvis man ønsker debug fra systemet
const bool DEBUG = true;

//"klokkefrekvensen" til systemet
const int DELAY = 200;

//Hvor lang tid servo 1 skal vente før den snur retning
const int SERVO1_TIME = 1000;


/* Indekser på joystick blir 0=opp, 1=hoyre, 2=ned, 3=venstre
 *    0
 *  3   1
 *    2
 */
int joystick[] = {22, 24, 26, 28};
const int UP = 0;
const int DOWN = 2;
const int RIGHT = 1;
const int LEFT = 3;

Servo servo1;
Servo hjul;
Servo kran;

int led_state = 0;
int servo1_val = 0;
int servo1_timer = 0;

void setup() {
  /*
   * Knappene og joysticken er programmert med pullup, så verdiene vil være aktiv lav.
   */
  pinMode(led, OUTPUT);
  pinMode(push_button, INPUT_PULLUP);

  for(int i = 0; i < 4; i++) {
    pinMode(joystick[i], INPUT_PULLUP);
  }

  servo1.attach(2);
  hjul.attach(3);
  kran.attach(4);

  Serial.begin(9600);
  digitalWrite(led, LOW);

  Serial.println("Ready!");
}

void loop() {
  digitalWrite(led, led_state);
  led_state = !led_state;
  
  int button_val = digitalRead(push_button);
  int joystick_val = read_joystick();

  if(DEBUG) {
    print_input(button_val, joystick_val);
  }

  ctrl_servo1();
  ctrl_hjul(button_val);
  ctrl_kran(joystick_val);


  delay(DELAY);
}

void ctrl_kran(int joystick) {
  if(joystick == UP) {
    kran.write(30);
  } else if(joystick == DOWN) {
    kran.write(130);
  } else {
    kran.write(90);
  }
}

void ctrl_hjul(int button) {
  /*
   * Styrer "hjulet" i midten av veggen
   */
  if(!button) {
    hjul.write(180);
  } else {
    hjul.write(90);
  }
}

void ctrl_servo1() {
  /*
   * Styrer den øverste løftemekanismen. 
   */
  if(servo1_timer >= SERVO1_TIME) {
    if(servo1_val == 0) {
      servo1_val = 180;
    } else {
      servo1_val = 0;
    }

    servo1_timer = 0;
  }

  servo1_timer += DELAY;
  servo1.write(servo1_val);
}

int read_joystick() {
  //Bare en kan fysisk være aktivert om gangen
  //Aktiv lav
  for(int i = 0; i < 4; i++) {
    if(digitalRead(joystick[i]) == LOW) {
      return i;
    }
  }
  return -1;
}

void print_input(int button, int joystick) {
  Serial.print("Button: ");
  Serial.print(!button);
  Serial.print("  Joystick: ");

  if(joystick == -1) {
    Serial.print("NULL");
  } else if(joystick == 0) {
    Serial.print("Opp");
  } else if(joystick == 1) {
    Serial.print("Høyre");
  } else if(joystick == 2) {
    Serial.print("Ned");
  } else if(joystick == 3) {
    Serial.print("Venstre");
  }
  Serial.println();
}

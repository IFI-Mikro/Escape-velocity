// Flash_Mode functions:
//int disabled_flash = 0; //Set value to 2 to instantly enable..
void flash_mode(int num_active_buttons){ //>input< THIS DECIDES A bit, so that changing it will adjust all other settings, in a perfectly fine way.
    
    //if (disabled_flash == 0) return;
    //else if (disabled_flash == 1) {disabled_flash++; return;}
    swipeAClear(wallLightRed, 14);
    clearClock();
    clearWall();
    // CONSTANTS/SETUP 
    //int const perfect_score_here = num_active_buttons*3;
    //int const worst_score_here = -(42 - num_active_buttons)*2 - num_active_buttons*3;
    
    bool that_pattern[42]; // to map the right buttons
    int next_button = 0; //to choose a button for the pattern
    bool not_at_all_used; //to see if the button is already inside the pattern 
    int my_countdown_num = num_active_buttons/3 + 5; // countdown value
    int clock_counting = 2; // added control
    for (int i = 0; i < 42; i++) that_pattern[i] = false; // It will store true when the button is used.....


    // GAME RULES
    // right gives +3
    // wrong gives -2
    // missed right gives -2
    // point modes: (positive) (negative) (max score) (zero)
    //kind of dont alter this.......

  

    // Generate and store a random image pattern
    next_button = int(random(smallMode, 42));//int(random(42));
    for (int myI = 0; myI<num_active_buttons; myI++){
      not_at_all_used = true;
      while(that_pattern[next_button]){
        next_button = int(random(smallMode, 42));//int(random(42));
      }
      that_pattern[next_button] = true;
      setPixel(next_button, wallBlue);
      if (clock_counting == 2) {
        clock_counting = 0;
        dispNumber(my_countdown_num, clockOrange);
        my_countdown_num--;
      } else {
        clock_counting++;
      }
      delay(333);   
    }

    // Show image pattern for 20 sec with countdown
    dispNumber(my_countdown_num, clockOrange);
    for (my_countdown_num = my_countdown_num -1; my_countdown_num > -1; my_countdown_num--){
      delay(999);
      dispNumber(my_countdown_num, clockOrange);
    }
     
    // Clear an wait 30 sec with countdown >>>>> Added responsiveness (Show pressed butons);
    delay(100);
    clearClock();
    // for (int i = 6; i >= 0; i--) swipe0Clear((i%2 == 1) ? wallRed : wallBlue, i%3); //very pretty but:::
    
    randomSwiping(7, 1, 100);
    // more confusion
    clearWall();
    long current;
    for (my_countdown_num = 10; my_countdown_num > -1; my_countdown_num--){ 
      current = millis();
      while ((millis() - current) < 200) trackButtons(wallOrange);
      dispNumber(my_countdown_num, clockOrange);
    }   

    checkPattern_flash(num_active_buttons, that_pattern); 
    clearWall();
}

void randomSwiping(int num, int delay_num, int delay_two) {
  uint32_t a_color = wallBlue;
  for (int j = 0; j<num; j++) {
    a_color = (a_color == wallBlue)? wallRed : wallBlue;
    switch(int(random(8))){
      case 0: swipeAClear(a_color, delay_num); break; case 1: swipeBClear(a_color, delay_num); break; case 2: swipeCClear(a_color, delay_num); break; case 3: swipeDClear(a_color, delay_num); break;
      case 4: midSwipeAClear(a_color, delay_num); break; case 5: midSwipeBClear(a_color, delay_num); break; case 6: midSwipeCClear(a_color, delay_num); break; case 7: midSwipeDClear(a_color, delay_num); break;
    }
    delay(delay_two);
  }
}




void storeButtons(bool allButtons[], uint32_t color) {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      setPixel(i, color);
      allButtons[i] = true;
    } else if (!allButtons[i]) {
      setPixel(i, wallOff);
    }
  }
}

void trackButtons(uint32_t color) {
  for ( int i = 0; i < 42; i++) {
    if ( digitalRead(button(i)) == 1) {
      setPixel(i, color);
    } else {
      setPixel(i, wallOff);
    }
  }
}

void checkPattern_flash(int num_active_buttons, bool that_pattern[] ){
    // to map all the pressed buttons:
    int right = 0;
    int wrong = 0;
    int missed = 0;
    bool button_used;
    bool button_value;

    //int right_pressed[num_active_buttons];

    //int missed_pressed[num_active_buttons];
    
    //int wrong_pressed[42 - num_active_buttons];

    bool buttons1[42];
    for (int i = 0; i < 42; i++) buttons1[i] = false; // just to make sure...
    for(int i = 0; i<5; i++) storeButtons(buttons1, clockPurple); //for singleplayer, just make all checks this. but for now, it's like this because olympiaden og samarbeid + enklere kode.
    clearClock();
    delay(1000);
    // Check pattern
    for (int myI = 0; myI<42; myI++){
       button_used = that_pattern[myI];
       button_value = buttons1[myI];
       if (button_used) {
        if (button_value) {
          //right_pressed[right] = myI;
          right++;
          setPixel(myI, wallGreen);
        } else {
          //missed_pressed[missed] = myI;
          missed++;
          setPixel(myI, wallRed);
        }
       } else if (button_value) { 
        //wrong_pressed[wrong] = myI;
        wrong++;
        setPixel(myI, wallLightRed);
       } else {
        flashPixel(myI, 1, wallGrey);
       }
    }

    

    /*
    //__Score Debug__:
    int butttt = 15;
    setPixel(butttt, wallBlue);
    dispNumber(right, wallGreen);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    butttt++;
    setPixel(butttt, wallBlue);
    dispNumber(missed, wallRed);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    butttt++;
    setPixel(butttt, wallBlue);
    dispNumber(wrong, wallOrange);
    while(digitalRead(button(butttt)) == LOW) delay(10);
    */

    
    // Show rights / wrongs / missed and display score
    int score = (3 * right) - (2 * wrong) - (2 * missed); 

    uint32_t this_color = wallBlue;
    button_value = false;
    if (score < 0) {
      this_color = wallRed;
      score = -score;
    } else if (score > 0) {
      if (score == num_active_buttons * 3) {
        button_value = true;
      }
      this_color = wallGreen;
    }
    if (score > 99) score = 99;
    long ohNo;
    for (int myI = 0; myI < score; myI++){
      dispNumber(myI, this_color);
      ohNo =  (long)((myI * 10) +((1000 * (myI*myI + 1))/(score*score+1)));
      if (ohNo < 200) delay(200);
      else if (ohNo > 2000) delay(2000);
      else delay(ohNo);
    }
    if (button_value) dispNumber(score, wallPurple); // you have achieved max score...
    else dispNumber(score, this_color);
    delay(5000);
    swipeBClear(wallGrey, 15);
}

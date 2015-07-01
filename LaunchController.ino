/*
  LaunchController
  
  Domenick Petrella
  May 2015
  
  The general operation this will enforce is:
   - All buttons are ignored until the Master Arm button is pressed
   - Once the Master Arm button is pressed, the Pad Select buttons can be active
   - Only one Pad Select button can be enabled at a time
   - The Launch button is able to be pressed after a Pad Select button is pressed
  
  If any button is held down more than 10 seconds (configurable), all
  buttons are reset.  In the case of the launch button, this may not matter,
  as it will send current down the chosed pad lead as soon as it's pressed.
  
  The loop() runs at a frequency set by delayTime, in milliseconds.  Each iteration
  if the loop will check for button states and perform actions as necessary.  This
  construct was chosen because all the buttons used were the momentary press type
  in order to keep the system fully digital, and not reliant upon mechanical latches
  in the buttons to determine if one had been pressed.

 */

// constants won't change. They're used here to
// set pin numbers:
const int masterArmButton = A0;
const int armOneButton    = A1;
const int armTwoButton    = A2;
const int armThreeButton  = A3;
const int armFourButton   = A4;
const int launchButton    = A5;

const int masterArmLed    = 2;
const int armOneLed       = 3;
const int armTwoLed       = 4;
const int armThreeLed     = 5;
const int armFourLed      = 6;
const int launchLed       = 7;

const int padOneControl   = 8;
const int padTwoControl   = 9;
const int padThreeControl = 10;
const int padFourControl  = 11;

// and these are used to decide how long a button needs to be pressed
// to consider it *really* pressed.
const int delayTime       = 50;  // milliseconds
const int pressedScore    = 2;   // multiply this with the delayTime to decide
                                 // how long a button needs to be pressed.
const int resetScore      = 200; // (1000/50)*10=200, for a 10 second time.
                                 // this value is used to determine when everything
                                 // should be reset in the event of a long press.
int       launchCounter   = 0;
const int launchCountMax  = 25;
const int launchDelay     = 100;

// The minimum analog input value.
const int minAnalogValue  = 700;

// since we're using analog inputs, these variables will receive
// the value from the analog input.
int masterArmButtonValue  = 0;
int armOneButtonValue     = 0;
int armTwoButtonValue     = 0;
int armThreeButtonValue   = 0;
int armFourButtonValue    = 0;
int launchButtonValue     = 0;

// the score is a value indicating how long a button has been pushed.
int masterArmButtonScore  = 0;
int armOneButtonScore     = 0;
int armTwoButtonScore     = 0;
int armThreeButtonScore   = 0;
int armFourButtonScore    = 0;
int launchButtonScore     = 0;

// the pressed booleans track is a button is currently pressed,
// and can be used to see if there has been a change in state.
boolean masterArmButtonPressed = false;
boolean armOneButtonPressed    = false;
boolean armTwoButtonPressed    = false;
boolean armThreeButtonPressed  = false;
boolean armFourButtonPressed   = false;
boolean launchButtonPressed    = false;

// when the logic decides a button is actually pressed, these variables
// are set.
boolean masterArmButtonPressAsserted = false;
boolean armOneButtonPressAsserted    = false;
boolean armTwoButtonPressAsserted    = false;
boolean armThreeButtonPressAsserted  = false;
boolean armFourButtonPressAsserted   = false;
boolean launchButtonPressAsserted    = false;

// this is used to run initial power on routines.
boolean initialPowerUp = true;

void setup() {
  // enable serial debugging, if you need it
//  Serial.begin(115200);
  
  // initialize the LED and control pins as an output:
  pinMode(masterArmLed,    OUTPUT);
  pinMode(armOneLed,       OUTPUT);
  pinMode(armTwoLed,       OUTPUT);
  pinMode(armThreeLed,     OUTPUT);
  pinMode(armFourLed,      OUTPUT);
  pinMode(launchLed,       OUTPUT);
  
  pinMode(padOneControl,   OUTPUT);
  pinMode(padTwoControl,   OUTPUT);
  pinMode(padThreeControl, OUTPUT);
  pinMode(padFourControl,  OUTPUT);
}

void loop() {
  
  // initial power up.  push the pad controls low, and
  // loop through the LEDs for a test.  and to look fancy.
  if (initialPowerUp == true) {
    initialPowerUp = false;
    
    resetAll();
  }
  
  // read the state of the pushbutton value:
  masterArmButtonValue = analogRead(masterArmButton);
  armOneButtonValue    = analogRead(armOneButton);
  armTwoButtonValue    = analogRead(armTwoButton);
  armThreeButtonValue  = analogRead(armThreeButton);
  armFourButtonValue   = analogRead(armFourButton);
  launchButtonValue    = analogRead(launchButton);
  
  // print buttonState for debugging
  // remember to uncomment the Serial.begin in setup()
//  Serial.println("Master value:");
//  Serial.println(masterArmButtonValue);
//  Serial.println(armOneButtonValue);
//  Serial.println(armTwoButtonValue);
//  Serial.println(armThreeButtonValue);
//  Serial.println(armFourButtonValue);
//  Serial.println(launchButtonValue);
  
  // Count how long the button is pushed, to make sure it's
  // for realzies, and see if this is part of a continuous push.
  // And also!  Do not pay attention to pad or launch buttons
  // unless the master arm button has been pressed.
  // ... the launch pad should also ignore everything until
  // *both* the master arm button and a pad button have been pressed.
  if (masterArmButtonValue >= minAnalogValue) {
     masterArmButtonScore++; masterArmButtonPressed = true; }
     else { masterArmButtonPressed = false; }
  if (armOneButtonValue    >= minAnalogValue && masterArmButtonPressAsserted == true) {
     armOneButtonScore++;    armOneButtonPressed    = true; }
     else { armOneButtonPressed    = false; }
  if (armTwoButtonValue    >= minAnalogValue && masterArmButtonPressAsserted == true) {
     armTwoButtonScore++;    armTwoButtonPressed    = true; }
     else { armTwoButtonPressed    = false; }
  if (armThreeButtonValue  >= minAnalogValue && masterArmButtonPressAsserted == true) {
     armThreeButtonScore++;  armThreeButtonPressed  = true; }
     else { armThreeButtonPressed  = false; }
  if (armFourButtonValue   >= minAnalogValue && masterArmButtonPressAsserted == true) {
     armFourButtonScore++;   armFourButtonPressed   = true; }
     else { armFourButtonPressed   = false; }
  if (launchButtonValue    >= minAnalogValue && masterArmButtonPressAsserted == true) {
     if (armOneButtonPressAsserted   == true ||
         armTwoButtonPressAsserted   == true ||
         armThreeButtonPressAsserted == true ||
         armFourButtonPressAsserted  == true) {
         
         launchButtonScore++; launchButtonPressed = true;
       }
     } 
     else { launchButtonPressed    = false; }
  
  // if any button is pressed more than 10 cumulative seconds, reset everything.
  if (masterArmButtonScore >= resetScore ||
      armOneButtonScore    >= resetScore ||
      armTwoButtonScore    >= resetScore ||
      armThreeButtonScore  >= resetScore ||
      armFourButtonScore   >= resetScore ||
      launchButtonScore    >= resetScore) {
    
     resetAll();
  }
  
  // First, deal with the master arm/reset button.
  // This one button rules them all.
  
  // a real press, and the master arm button was off.  arm the panel.
  if (masterArmButtonScore >= pressedScore &&
      masterArmButtonPressAsserted == false && 
      masterArmButtonPressed == false) {
    masterArmButtonScore = 0;
    masterArmButtonPressAsserted = true;
    digitalWrite(masterArmLed, HIGH);
  }
  
  // a real press, and the master arm button is already on.  reset everything.
  if (masterArmButtonScore >= pressedScore &&
      masterArmButtonPressAsserted == true &&
      masterArmButtonPressed == false) {
    
      resetAll();
  }
  
  // a real press, and the arm one button was off.
  // arm pad one, disarm all others, turn on launch led.
  if (armOneButtonScore >= pressedScore &&
      armOneButtonPressAsserted == false && 
      armOneButtonPressed == false &&
      masterArmButtonPressAsserted == true) {
        
    armOneButtonScore = 0;
    armOneButtonPressAsserted = true;
    digitalWrite(armOneLed, HIGH);
    
    armTwoButtonScore = 0;
    armTwoButtonPressAsserted = false;
    digitalWrite(armTwoLed, LOW);
    
    armThreeButtonScore = 0;
    armThreeButtonPressAsserted = false;
    digitalWrite(armThreeLed, LOW);
    
    armFourButtonScore = 0;
    armFourButtonPressAsserted = false;
    digitalWrite(armFourLed, LOW);
    
    digitalWrite(launchLed, HIGH);
  }
  
  // a real press, and the arm two button was off.
  // arm pad two, disarm all others, turn on launch led.
  if (armTwoButtonScore >= pressedScore &&
      armTwoButtonPressAsserted == false && 
      armTwoButtonPressed == false &&
      masterArmButtonPressAsserted == true) {
        
    armOneButtonScore = 0;
    armOneButtonPressAsserted = false;
    digitalWrite(armOneLed, LOW);
    
    armTwoButtonScore = 0;
    armTwoButtonPressAsserted = true;
    digitalWrite(armTwoLed, HIGH);
    
    armThreeButtonScore = 0;
    armThreeButtonPressAsserted = false;
    digitalWrite(armThreeLed, LOW);
    
    armFourButtonScore = 0;
    armFourButtonPressAsserted = false;
    digitalWrite(armFourLed, LOW);
    
    digitalWrite(launchLed, HIGH);
  }
  
  // a real press, and the arm three button was off.
  // arm pad three, disarm all others, turn on launch led.
  if (armThreeButtonScore >= pressedScore &&
      armThreeButtonPressAsserted == false && 
      armThreeButtonPressed == false &&
      masterArmButtonPressAsserted == true) {
        
    armOneButtonScore = 0;
    armOneButtonPressAsserted = false;
    digitalWrite(armOneLed, LOW);
    
    armTwoButtonScore = 0;
    armTwoButtonPressAsserted = false;
    digitalWrite(armTwoLed, LOW);
    
    armThreeButtonScore = 0;
    armThreeButtonPressAsserted = true;
    digitalWrite(armThreeLed, HIGH);
    
    armFourButtonScore = 0;
    armFourButtonPressAsserted = false;
    digitalWrite(armFourLed, LOW);
    
    digitalWrite(launchLed, HIGH);
  }
  
  // a real press, and the arm four button was off.
  // arm pad four, disarm all others, turn on launch led.
  if (armFourButtonScore >= pressedScore &&
      armFourButtonPressAsserted == false && 
      armFourButtonPressed == false &&
      masterArmButtonPressAsserted == true) {
        
    armOneButtonScore = 0;
    armOneButtonPressAsserted = false;
    digitalWrite(armOneLed, LOW);
    
    armTwoButtonScore = 0;
    armTwoButtonPressAsserted = false;
    digitalWrite(armTwoLed, LOW);
    
    armThreeButtonScore = 0;
    armThreeButtonPressAsserted = false;
    digitalWrite(armThreeLed, LOW);
    
    armFourButtonScore = 0;
    armFourButtonPressAsserted = true;
    digitalWrite(armFourLed, HIGH);
    
    digitalWrite(launchLed, HIGH);
  }
  
  // master arm button is enabled, and the launch button is pressed.
  // sequentially find the armed pad, and LAUNCH IT
  // this one doesn't have the check if the button was released.
  if (launchButtonScore >= pressedScore &&
      masterArmButtonPressAsserted == true) {
    
    if (armOneButtonPressAsserted   == true) {
      doLaunch(padOneControl, armOneLed);
    }
    
    if (armTwoButtonPressAsserted   == true) {
      doLaunch(padTwoControl, armTwoLed);
    }
    
    if (armThreeButtonPressAsserted == true) {
      doLaunch(padThreeControl, armThreeLed);
    }
    
    if (armFourButtonPressAsserted  == true) {
      doLaunch(padFourControl, armFourLed);
    }
    
  }
  
  // This is really important.  The scoring mechanism depends on the
  // timing to count accurately.
  delay(delayTime);
}

void doLaunch(int launchPadControl, int launchPadLed) {
    // apply current to pad one for five seconds
  digitalWrite(launchPadControl, HIGH);
  launchCounter = 0;
  // flash the pad and launch leds while current is being applied.
  do {
    digitalWrite(launchPadLed, LOW);
    digitalWrite(launchLed, LOW);
    delay(launchDelay);
    digitalWrite(launchPadLed, HIGH);
    digitalWrite(launchLed, HIGH);
    delay(launchDelay);
    launchCounter++;
  } while (launchCounter <= launchCountMax);
  digitalWrite(launchPadControl, LOW);
  // now, reset everything.
  resetAll();
}

void resetAll() {
  
  Serial.println("===== RESETTING ALL =====");
  
  masterArmButtonScore = 0;
  armOneButtonScore    = 0;
  armTwoButtonScore    = 0;
  armThreeButtonScore  = 0;
  armFourButtonScore   = 0;
  launchButtonScore    = 0;
  
  masterArmButtonPressAsserted = false;
  armOneButtonPressAsserted    = false;
  armTwoButtonPressAsserted    = false;
  armThreeButtonPressAsserted  = false;
  armFourButtonPressAsserted   = false;
  launchButtonPressAsserted    = false;
  
  digitalWrite(padOneControl,   LOW);
  digitalWrite(padTwoControl,   LOW);
  digitalWrite(padThreeControl, LOW);
  digitalWrite(padFourControl,  LOW);
  
  for (int testLed = 2; testLed <= 7; testLed++) {
    digitalWrite(testLed, LOW);
  }
  delay(200);
  
  for (int testLed = 2; testLed <= 7; testLed++) {
    digitalWrite(testLed, HIGH);
    delay(100);
    digitalWrite(testLed, LOW);
  }
  for (int ledTestLoop = 0; ledTestLoop <= 2; ledTestLoop++) {
    delay(100);
    for (int testLed = 2; testLed <= 7; testLed++) {
      digitalWrite(testLed, HIGH);
    }
    delay(100);
    for (int testLed = 2; testLed <= 7; testLed++) {
      digitalWrite(testLed, LOW);
    }
  }
  
}

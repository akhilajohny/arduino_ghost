// Define pin connections & motor's steps per revolution
const int dirPin = 5; //dirPin to driver, see pololu product 2134 for more information 
const int stepPin = 4; //stepPin to driver, see pololu product 2134 for more information
const int SelectorM0 = 6; //M0 pin to driver, see pololu product 2134 for more information
const int SelectorM1 = 7; //M1 pin to driver, see pololu product 2134 for more information
const int sleep = 8; //sleep pin to driver, see pololu product 2134 for more information
const int istart = 18; //pin for button detection for start/pause
const int irot = 19; //pin for button detection for direction of rotation
const int inum = 21; //pin for button detection for changing the number in pulse per microsecond
const int iplace = 20; //pin for button detection for changing the placevalue in pulse per microsecond
const char *rotdisp[] = {"CW rotation","CCW rotation"}; //array to display direction of rotation

// libraries for OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// defining screen size
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//State Machine
#define pause 1 //pause state for pausing motor and changing rotation parameters
#define go 2 //rotating state for motor, cannot change parameters in this state
int state = 1;


// variables to control driver
const int stepsPerRevolution = 200; // defines length of travel in steps per revolution, 200 steps = 1 revolution
int arspeed[] = {2,0,0,0}; // defines speed of rotation, frequency of pulse in microseconds, in array form for display and manipulation
const int M0 = HIGH; // Controls microstep resolution, see pololu product 2134 for more information
const int M1 = HIGH; // Controls microstep resolution, see pololu product 2134 for more information
int rspeed = 2000; //defines speed of rotation, frequency of pulse in microsecond, in int form for driver control


//variables for code logic
int rstate = 0; //value to index the array which displays rotation direction(rotdisp)
int numstate = 2; //value of number being edited
int placestate = 0; //placevalue of number being edited (where 0 is thousands place, 3 is ones place), also value to index array of speed (arspeed)

  
void setup() {
  Serial.begin(9600);

  //Declare pins as Outputs for motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(SelectorM0, OUTPUT);
  digitalWrite(SelectorM0, M0);
  pinMode(SelectorM1, OUTPUT);
  digitalWrite(SelectorM1, M1);
  pinMode(sleep, OUTPUT);
  digitalWrite(sleep,HIGH);
  pinMode(istart, INPUT_PULLUP);
  pinMode(irot, INPUT_PULLUP);
  pinMode(inum, INPUT_PULLUP);
  pinMode(iplace,INPUT_PULLUP);

  // initialize OLED display with address 0x3C for 128x64
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
   Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  delay(2000);         // wait for initializing
  
  //Display sequence
  display.clearDisplay();          // clear display
  display.setTextSize(1);          // text size
  display.setTextColor(WHITE);     // text color
  display.setCursor(0, 10);        // position to display
  display.println("Starting");     // text to display
  display.display();               // show on OLED


  
  // Display state of motore, frequency of pulses per microseconds, direction of rotation
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,10);
      display.println("Paused");
      display.setCursor(0, 20);
      display.println("Pulse per microsecond");
      for (int i = 0; i<4;i++){  //have this for loop to keep '0' as place holders
        display.setCursor(i*6,30);
        display.print(arspeed[i]);
      }
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);
      display.display();               // show on OLED

  Serial.write("Beging\n");
}

void loop() {
    while (state == pause){
      //display sequence
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(0,10);
        display.println("Paused");
        display.setCursor(0, 20);
        display.println("Pulse per microsecond");
        for (int i = 0; i<4;i++){
          display.setCursor(i*6,30);
          display.print(arspeed[i]);
        }
        display.setCursor(0,50);
        display.print(rotdisp[rstate]);
        display.setCursor(placestate*6,32);
        display.print("_");
        display.display();


      //button detection
      int startVal = digitalRead(istart);
      int rotVal = digitalRead(irot);
      int numVal = digitalRead(inum);
      int placeVal = digitalRead(iplace);


      //Changes the pulse per microdecond based on button inputs
      numstate = arspeed[placestate];
      if (numVal == 0){ //number changing logic
        if (numstate == 9){
          numstate = 0;
          delay(250); //button delay for user comfort
        }
        else{
        numstate = numstate+1;
        delay(250);
        }
      arspeed[placestate] = numstate;
      rspeed = arspeed[0]*1000 + arspeed[1]*100 + arspeed[2]*10 + arspeed[3]*1; //takes the array and converts it to int
      }

      if (placeVal == 0){ //place value changing logic
        if (placestate == 3){
          placestate = 0;
          delay(250);
        }
        else{
          placestate = placestate+1;
          delay(250);
        }
      }


      //Changes the rotation direction based on button input
      if (rotVal == 0){
        if (rstate == 0){
          rstate = 1;
          digitalWrite(dirPin, LOW); //set to counter clockwise
          delay(250);
        }
        else if (rstate == 1){
          rstate = 0;
          digitalWrite(dirPin, HIGH); //set to clockwise
          delay(250);
        }
      }


      //Changes state to go when button is activated
      if (startVal == 0){
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,20);
          display.println("Starting");
          display.display();
          digitalWrite(sleep, HIGH);
          Serial.write("Turning On\n");
          state = 2;
          delay(1000); 
        }
      
      delay(50); //refresh rate for pause state, kept low to increase responsiveness of buttons
    } //end of case pause
    
    
    
    while (state == go){
      // Spins motor
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(rspeed);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(rspeed);
      }


      // Display sequence
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,10);
      display.println("On");
      display.setCursor(0, 20);
      display.println("Pulse per microsecond");
      for (int i = 0; i<4;i++){
      display.setCursor(i*6,30);
      display.print(arspeed[i]);
      }
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);
      display.display();
    

      //Changes state to pause when button is activated
      int startVal = digitalRead(istart);
      if (startVal == 0){
          display.clearDisplay();
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(0,20);
          display.println("pausing");
          display.display();
          digitalWrite(sleep, LOW);
          Serial.write("Turning Off\n");
          state = 1;
          delay(1000);
        }
      delay(1000); // Refresh rate for go state, also controls motor stop duration, change to 0 for continuous rotation
    } //end of case go

 
}

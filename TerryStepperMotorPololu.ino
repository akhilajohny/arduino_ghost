// Define pin connections & motor's steps per revolution
const int dirPin = 5;
const int stepPin = 4;
const int SelectorM0 = 6;
const int SelectorM1 = 7;
const int sleep = 8;
const int istart = 18;
const int ostart = 10;
const int irot = 19;
const int orot = 16;
char *rotdisp[] = {"CW rotation","CCW rotation"};

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
#define pause 1
#define go 2
int state = 1;


// User input
const int stepsPerRevolution = 200; // defines length of travel in steps per revolution, 200 steps = 1 revolution
const int speedc = 2000; // defines speed of clockwise rotation, frequency of pulse in microseconds
const int speedcc = 1000; // defines spped of counter-clockwise rotation, frequency of pulse in microseconds
const int M0 = HIGH; // Controls microstep resolution, see pololu product 2134 for more information
const int M1 = HIGH; // Controls microstep resolution, see pololu product 2134 for more information


//variables
int rstate = 0;

  
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
  pinMode(ostart, OUTPUT);
  digitalWrite(ostart, HIGH); 
  pinMode(istart, INPUT);
  pinMode(orot, OUTPUT);
  digitalWrite(orot, HIGH);
  pinMode(irot, INPUT);

  // initialize OLED display with address 0x3C for 128x64
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
   Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }
  delay(2000);         // wait for initializing
  display.clearDisplay(); // clear display

  display.setTextSize(1);          // text size
  display.setTextColor(WHITE);     // text color
  display.setCursor(0, 10);        // position to display
  display.println("Starting");     // text to display
  display.display();               // show on OLED


  
  // Display frequency of pulses per microseconds
      display.clearDisplay(); // clear display
      display.setTextSize(1);          // text size
      display.setTextColor(WHITE);     // text color
      display.setCursor(0,10);
      display.println("Paused");
      display.setCursor(0, 20);        // position to display
      display.println("Pulse per microsecond");
      display.setCursor(0,30);
      display.print(speedc);     // text to display
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);
      display.display();               // show on OLED

  Serial.write("Beging\n");
}

void loop() {
    while (state == pause){
      display.clearDisplay(); // clear display
      display.setTextSize(1);          // text size
      display.setTextColor(WHITE);     // text color
      display.setCursor(0,10);
      display.println("Paused:::");
      display.setCursor(0, 20);        // position to display
      display.println("Pulse per microsecond");
      display.setCursor(0,30);
      display.print(speedc);     // text to display
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);
      display.display();               // show on OLED

      int startVal = analogRead(istart);
      Serial.write("startval: ");
      Serial.println(startVal);
      int rotVal = analogRead(irot);
      Serial.write("rotVal: ");
      Serial.println(rotVal);
      Serial.write("dir: ");
      Serial.println(rstate);

      if (rotVal == 1023){
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


      if (startVal == 1023){
          display.clearDisplay(); // clear display
          display.setTextSize(2);          // text size
          display.setTextColor(WHITE);     // text color
          display.setCursor(0,10);
          display.println("Starting");
          display.display();
          digitalWrite(sleep, HIGH);
          Serial.write("Turning On\n");
          state = 2;
          delay(1000);
        }


      
      delay(50); //refresh rate for pause
    } //end of case pause
    
    
    
    
    while (state == go){
      // Spin motor slowly
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speedc);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speedc);
      }

      // Display frequency of pulses per microseconds
      display.clearDisplay(); // clear display
      display.setTextSize(1);          // text size
      display.setTextColor(WHITE);     // text color
      display.setCursor(0,10);
      display.println("rotating");
      display.setCursor(0, 20);        // position to display
      display.println("Pulse per microsecond");
      display.setCursor(0,30);
      display.print(speedc);     // text to display
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);
      display.display();               // show on OLED

    
    
    
      delay(1000); // Wait a second
  
      // Spin motor quickly
      for(int x = 0; x < stepsPerRevolution; x++)
      {
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(speedcc);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(speedcc);
      }

      // Display frequency of pulses per microseconds
      display.clearDisplay(); // clear display
      display.setTextSize(1);          // text size
      display.setTextColor(WHITE);     // text color
      display.setCursor(0,10);
      display.println("rotating");
      display.setCursor(0, 20);        // position to display
      display.println("Pulse per microsecond: ");
      display.setCursor(0,30);
      display.print(speedcc);     // text to display
      display.setCursor(0,50);
      display.print(rotdisp[rstate]);       
      display.display();               // show on OLED
 
    
      delay(1000); // Wait a second

      int startVal = digitalRead(istart);
      int sleepVal = digitalRead(sleep);
      Serial.write("startval: ");
      Serial.println(startVal);
      
      if (startVal == 1){
          display.clearDisplay(); // clear display
          display.setTextSize(3);          // text size
          display.setTextColor(WHITE);     // text color
          display.setCursor(0,10);
          display.println("pausing");
          display.display();
          digitalWrite(sleep, LOW);
          Serial.write("Turning Off\n");
          state = 1;
          delay(1000);
        }
    } //end of case go

  
  
}


/*

v0.01 
- Initial version

v0.02 
- Added joystick position indicators in MCM. 
- Added code controllilng pin outputs based upon joystick values in MCM

v0.03 
- Added error messages if controller error reported during startup.
- Added pulse counters to MCM

v0.04
- Cleaned code
- Added Roll and Slide sections to the ACS

v0.05
- Added summary screen to the end of the ACS

v0.06
- Added confirmation screen to end of the ACS
- Added screen to set Direction and Resolution output pin states before entering Automatic Run Mode

v0.07
- Commented out sections in MCM which display pulse counters. Now not needed except for debug now that motor outputs are working
- Finished ARP to control head based upon settings defined in the ACS

*/

#include <LiquidCrystal.h> // include the library to run the LCD
#include <PS2X_lib.h>  //for v1.6


PS2X ps2x; // create PS2 Controller Class

 /*
 Right now, the library does NOT support hot pluggable controllers, meaning 
 you must always either restart your Arduino after you conect the controller, 
 or call config_gamepad(pins) again after connecting the controller.
 */

int error = 0; 
byte type = 0;
byte vibrate = 0;
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // initialize the LCD with the correct pin numbers (RS, EN, D4, D5, D6, D7)

int manualResolution = 2; //stores the stepping resolution when in MCM
int manualExit = 0; //Sets the value used to decide whether to exit the MCM while statement
int manualDisplay = 0; //stores the value that defines whether to display the stick X/Y values or the step counter in MCM
int manualPanInc = 0; //holds the total number of steps with PanDir = High in the MCM
int manualPanDec = 0; //holds the total number of steps with PanDir = Low in the MCM
int manualTiltInc = 0;
int manualTiltDec = 0;
int manualRollInc = 0;
int manualRollDec = 0;
int manualSlideInc = 0;
int manualSlideDec = 0;

int menuExit = 0; //sets the value used to decide whether to exit the main menu while statement
int menuSelect = 0; //sets the value used to specify which main menu option (manual or automatic) is chosen

int autoPanRes = 2; //holds the value refering to the pan resolution selected in the ACS
int autoTiltRes = 2;
int autoRollRes = 2;
int autoSlideRes = 2; 

int autoPanEnable = 0; //holds the value determining whether we move in this axis
int autoTiltEnable = 0;
int autoRollEnable = 0;
int autoSlideEnable = 0;

int autoPanDir = 0; //holds the value determing what direction we travel in if travel in this axis is set
int autoTiltDir = 0;
int autoRollDir = 0;
int autoSlideDir = 0;

int autoPanIntSet = 2; //setup values used to display and determine stepping timing intervals in particular axis
int autoTiltIntSet = 2;
int autoRollIntSet = 2;
int autoSlideIntSet = 2;

int autoPanInt = 0; //values holding interval between setps (in milliseconds)
int autoTiltInt = 0;
int autoRollInt = 0;
int autoSlideInt = 0;

int autoRunTime = 0;
float autoRunTimeHours = 0;
unsigned long autoRunTimeMS = 0;
int autoMenuNext = 0;
unsigned long autoRunStart = 0;
unsigned long autoRunTimeClock = 0;
unsigned long autoPanNext = 0;
unsigned long autoTiltNext = 0;
unsigned long autoRollNext = 0;
unsigned long autoSlideNext = 0;
unsigned long autoPanPrev = 0;
unsigned long autoTiltPrev = 0;
unsigned long autoRollPrev = 0;
unsigned long autoSlidePrev = 0;


const int panDir = 39; // the following sections sets the pin numbers for the outputs to the motor control boards
const int panStep = 37;
const int panRes1 = 31;
const int panRes2 = 33;
const int panRes3 = 35;

const int tiltDir = 48;
const int tiltStep = 46;
const int tiltRes1 = 40;
const int tiltRes2 = 42;
const int tiltRes3 = 44;

const int rollDir = 38;
const int rollStep = 36;
const int rollRes1 = 30;
const int rollRes2 = 32;
const int rollRes3 = 34;

const int slideDir = 49;
const int slideStep = 47;
const int slideRes1 = 41;
const int slideRes2 = 43;
const int slideRes3 = 45;


void setup(){
  Serial.begin(57600);

  lcd.begin(20, 4); // set up the LCD's number of columns and rows:
  lcd.setCursor(0,0); //write welcome message to LCD
  lcd.print("********************");
  lcd.setCursor(0,1);
  lcd.print("*                  *");
  lcd.setCursor(0,2);
  lcd.print("*                  *");
  lcd.setCursor(0,3);
  lcd.print("********************");
  lcd.setCursor (5,1);
  lcd.print("Welcome to");
  lcd.setCursor (4,2);
  lcd.print ("CAMCON v0.07");
  delay (3000);
  
  error = ps2x.config_gamepad(13,11,10,12, false, false);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  if(error == 0){
    Serial.println("Found Controller, configured successful");
}

  else if(error == 1){
    Serial.println("No controller found, check wiring");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Controller Error");
  lcd.setCursor(2,2);
  lcd.print("Please check and");
  lcd.setCursor(4,3);
  lcd.print("reset system");
  }
  
  else if(error == 2){
    Serial.println("Controller found but not accepting commands");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Controller Error");
  lcd.setCursor(2,2);
  lcd.print("Please check and");
  lcd.setCursor(4,3);
  lcd.print("reset system");
  }
  
  else if(error == 3){
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Controller Error");
  lcd.setCursor(2,2);
  lcd.print("Please check and");
  lcd.setCursor(4,3);
  lcd.print("reset system");
  }
  
  //Serial.print(ps2x.Analog(1), HEX);

  type = ps2x.readType(); 
  switch(type) {
  case 0:
    Serial.println("Unknown Controller type");
    break;
  case 1:
    Serial.println("DualShock Controller Found");
    break;
  case 2:
    Serial.println("GuitarHero Controller Found");
    break;
  }
  pinMode(panRes1, OUTPUT); // set stepper resolution pins as outputs
  pinMode(panRes2, OUTPUT);
  pinMode(panRes3, OUTPUT);
  pinMode(tiltRes1, OUTPUT);
  pinMode(tiltRes2, OUTPUT);
  pinMode(tiltRes3, OUTPUT);
  pinMode(rollRes1, OUTPUT);
  pinMode(rollRes2, OUTPUT);
  pinMode(rollRes3, OUTPUT);
  pinMode(slideRes1, OUTPUT);
  pinMode(slideRes2, OUTPUT);
  pinMode(slideRes3, OUTPUT);
  
  pinMode(panStep, OUTPUT); //set stepper step pins as outputs
  pinMode(tiltStep, OUTPUT);
  pinMode(rollStep, OUTPUT);
  pinMode(slideStep, OUTPUT);  
  
  pinMode(panDir, OUTPUT); //set stepper direction pins as outputs
  pinMode(tiltDir, OUTPUT);
  pinMode(rollDir, OUTPUT);
  pinMode(slideDir, OUTPUT);  
}

void loop(){
  if(error == 1) //skip loop if no controller found
    return; 
    
  //MAIN MENU STARTS HERE
 
 menuExit = 0; //resets the menu select variable so we will enter the while loop below which asks whether we want the enter the MCM or ACS 
 menuSelect = 0;
 
 digitalWrite(panStep, LOW); // set step pins low to prevent motors from moving 
 digitalWrite(tiltStep, LOW);
 digitalWrite(rollStep, LOW);
 digitalWrite(slideStep, LOW);
 
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Press Square for");
 lcd.setCursor(0,1);
 lcd.print("Manual Control Mode"); 
 lcd.setCursor(0,2);
 lcd.print("Press Circle for");
 lcd.setCursor(0,3);
 lcd.print("Automatic Mode Setup");
 while(menuExit == 0){
   ps2x.read_gamepad();
   if(ps2x.ButtonPressed(PSB_PINK)){ //square has been pressed and therefore we go to the Manual Control Mode
     menuSelect = 1;
     menuExit = 1;
   }
   
   if(ps2x.ButtonPressed(PSB_RED)){ //circle has been pressed and therefore we go to Automatic Control Setup 
     menuSelect = 2;
     menuExit = 1;
   }
 }
  
  //MANUAL CONTROL STARTS HERE
  
  if(menuSelect == 1){ //start of if statement that is true if Manual Control Mode was selected in the Main Menu
    menuSelect = 0; //this will ensure that we are returned to the start of the loop() function and not dumped into Automatic Control Setup when Manual Control Mode is exited
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Entering Manual Mode");
    lcd.setCursor(0,1);
    lcd.print("Square for Stick X/Y");
 // lcd.setCursor(0,2);
 // lcd.print("Circle for Step Vals");
    lcd.setCursor(0,3);
    lcd.print("Triangle to Exit");
    delay(2000);
    lcd.clear();
    manualExit = 0;
    lcd.setCursor(0,0);
    lcd.print("Full Step Mode"); //prints the stepping mode on the LCD which is Full Step Mode by default
    
    while(manualExit == 0){
      lcd.setCursor(0,3);
      lcd.print ("Triangle to Exit");
      ps2x.read_gamepad();
      /* 
      You must Read Gamepad to get new values
      Read GamePad and set vibration values
      ps2x.read_gamepad(small motor on/off, larger motor strength from 0-255)
      if you don't enable the rumble, use ps2x.read_gamepad(); with no values
      you should call this at least once a second
      */
  
      /* Man Resolution values and the mode that they refer to 
       2 = Full Stepping
       3 = Half Stepping
       4 = Quarter Stepping
       5 = Eighth Stepping
       6 = Sixteenth Stepping
       */
        
        if(ps2x.ButtonPressed(PSB_PINK)){ //set up display to show stick L/R values
          manualDisplay = 1;
          lcd.setCursor(0,1);
          lcd.print("    LeftX     RightX");
          lcd.setCursor(0,2);
          lcd.print("    LeftY     RightY");
      }
        
    /*    if(ps2x.ButtonPressed(PSB_RED)){ // set up display to show pulse counters
          manualDisplay = 2;
          lcd.setCursor(0,1);
          lcd.print("        LX        RX");
          lcd.setCursor(0,2);
          lcd.print("        LY        RY");
        } */
        
        if(ps2x.ButtonPressed(PSB_BLUE)){ //clear display of stick L/R values and / or pulse counters
          manualDisplay = 0;
          lcd.setCursor(0,1);
          lcd.print("                    ");
          lcd.setCursor(0,2);
          lcd.print("                    ");
        }
        
        if(ps2x.ButtonPressed(PSB_L1)){ //if the L1 button is pressed then the resolution value is increased by a value of 1 (this moves from full step to half step etc) This if statement is only entered if the L1 button is pressed
            manualResolution = manualResolution + 1;
                   
                    if(manualResolution == 7){ //if the resolution value is 7 then the value wraps around to equal 2
                       manualResolution = 2;
                    }
                    
              if(manualResolution == 2){
                lcd.setCursor (0,0);
                lcd.print("Full Step Mode      ");
                digitalWrite(panRes1, LOW); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, LOW);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, LOW); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, LOW);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, LOW); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, LOW);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, LOW); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, LOW);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 3){
                lcd.setCursor (0,0);
                lcd.print("Half Step Mode      ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, LOW);
                digitalWrite(panRes3, LOW);                
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, LOW);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, LOW);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, LOW);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 4){
                lcd.setCursor (0,0);
                lcd.print("Quarter Step Mode   ");
                digitalWrite(panRes1, LOW); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, LOW); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, LOW); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, LOW); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 5){
                lcd.setCursor (0,0);
                lcd.print("Eighth Step Mode    ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 6){
                lcd.setCursor (0,0);
                lcd.print("Sixteenth Step Mode ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, HIGH);
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, HIGH);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, HIGH);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, HIGH);
              }
              
              else{
                lcd.setCursor (0,0);
                lcd.print("Resolution Error    ");
              }                                        
        }
    
         if(ps2x.ButtonPressed(PSB_R1)){ //if the R1 button is pressed then the resolution value is decreased by a value of 1 (this moves from half step to full step etc) This if statement is only entered if the R! button is pressed
            manualResolution = manualResolution - 1;
                  
                    if(manualResolution == 1){ //if the resolution value is 1 then the value wraps around to equal 6
                       manualResolution = 6;
                    }
                    
              if(manualResolution == 2){
                lcd.setCursor (0,0);
                lcd.print("Full Step Mode      ");
                digitalWrite(panRes1, LOW); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, LOW);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, LOW); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, LOW);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, LOW); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, LOW);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, LOW); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, LOW);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 3){
                lcd.setCursor (0,0);
                lcd.print("Half Step Mode      ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, LOW);
                digitalWrite(panRes3, LOW);                
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, LOW);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, LOW);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, LOW);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 4){
                lcd.setCursor (0,0);
                lcd.print("Quarter Step Mode   ");
                digitalWrite(panRes1, LOW); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, LOW); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, LOW); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, LOW); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 5){
                lcd.setCursor (0,0);
                lcd.print("Eighth Step Mode    ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, LOW);
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, LOW);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, LOW);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, LOW);
              }
              
              else if(manualResolution == 6){
                lcd.setCursor (0,0);
                lcd.print("Sixteenth Step Mode ");
                digitalWrite(panRes1, HIGH); //set the output pins HIGH or LOW to set pan step resolution
                digitalWrite(panRes2, HIGH);
                digitalWrite(panRes3, HIGH);
                digitalWrite(tiltRes1, HIGH); //set the output pins HIGH or LOW to set tilt step resolution
                digitalWrite(tiltRes2, HIGH);
                digitalWrite(tiltRes3, HIGH);
                digitalWrite(rollRes1, HIGH); //set the output pins HIGH or LOW to set roll step resolution
                digitalWrite(rollRes2, HIGH);
                digitalWrite(rollRes3, HIGH);
                digitalWrite(slideRes1, HIGH); //set the output pins HIGH or LOW to set slide step resolution
                digitalWrite(slideRes2, HIGH);
                digitalWrite(slideRes3, HIGH);
              }
              
              else{
                lcd.setCursor (0,0);
                lcd.print("Resolution Error    ");
              }                    
        }       
        
      //the following section displays the X and Y values for the two analog sticks
        
      if(manualDisplay == 1){

        if(ps2x.Analog(PSS_LX) < 100){ //if the number is less than 100 then we need to print a space over what was the 3rd character
          lcd.setCursor(2,1);
          lcd.print(" ");
        }
        if(ps2x.Analog(PSS_LX) < 10){ //if the number is less than 10 then we need to print a space over what was the 2nd and 3rd characters
          lcd.setCursor(1,1);
          lcd.print("  ");
        }
        lcd.setCursor(0,1);
        lcd.print(ps2x.Analog(PSS_LX));
 
    
        if(ps2x.Analog(PSS_LY) < 100){ //if the number is less than 100 then we need to print a space over what was the 3rd character
          lcd.setCursor(2,2);
          lcd.print(" ");
        }
        if(ps2x.Analog(PSS_LY) < 10){ //if the number is less than 10 then we need to print a space over what was the 2nd and 3rd characters
          lcd.setCursor(1,2);
          lcd.print("  ");
        }
        lcd.setCursor(0,2);
        lcd.print(ps2x.Analog(PSS_LY));
    
    
        if(ps2x.Analog(PSS_RX) < 100){ //if the number is less than 100 then we need to print a space over what was the 3rd character
          lcd.setCursor(12,1);
          lcd.print(" ");
        }
        if(ps2x.Analog(PSS_RX) < 10){ //if the number is less than 10 then we need to print a space over what was the 2nd and 3rd characters
          lcd.setCursor(11,1);
          lcd.print("  ");
        }      
        lcd.setCursor(10,1);
        lcd.print(ps2x.Analog(PSS_RX));
    
    
        if(ps2x.Analog(PSS_RX) < 100){ //if the number is less than 100 then we need to print a space over what was the 3rd character
          lcd.setCursor(12,2);
          lcd.print(" ");
        }
        if(ps2x.Analog(PSS_RY) < 10){ //if the number is less than 10 then we need to print a space over what was the 2nd and 3rd characters
          lcd.setCursor(11,2);
          lcd.print("  ");
        }
        lcd.setCursor(10,2);
        lcd.print(ps2x.Analog(PSS_RY));
      
      }
      
  /*    if(manualDisplay == 2){ //displays the number of steps
        lcd.setCursor(0,1);
        lcd.print(manualPanDec);
        lcd.setCursor(4,1);
        lcd.print(manualPanInc);
        lcd.setCursor(0,2);
        lcd.print(manualTiltDec);
        lcd.setCursor(4,2);
        lcd.print(manualTiltInc);
        lcd.setCursor(11,1);
        lcd.print(manualSlideDec);
        lcd.setCursor(15,1);
        lcd.print(manualSlideInc);        
        lcd.setCursor(11,2);
        lcd.print(manualRollDec);
        lcd.setCursor(15,2);
        lcd.print(manualRollInc);
      } */
      
      
 // This section moves the motors depending on the stick positions
            
        if(ps2x.Analog(PSS_LX) < 30){ //if left stick X value is less than 100 (i.e moved left) then set pan direction to left and step once)
          digitalWrite(panDir, LOW);
          digitalWrite(panStep, HIGH);
          delay(1);
          digitalWrite(panStep, LOW);
       // manualPanDec = manualPanDec + 1;
        }
        
        if(ps2x.Analog(PSS_LX) > 225){ //if left stick X value is more than 155 (i.e moved right) then set pan direction to right and step once)
          digitalWrite(panDir, HIGH);
          digitalWrite(panStep, HIGH);
          delay(1);
          digitalWrite(panStep, LOW);
       // manualPanInc = manualPanInc + 1;  
        }  
  
        if(ps2x.Analog(PSS_LY) < 30){ //if left stick Y value is less than 100 (i.e moved up) then set tilt direction to up and step once)
          digitalWrite(tiltDir, LOW);
          digitalWrite(tiltStep, HIGH);
          delay(1);
          digitalWrite(tiltStep, LOW);
       // manualTiltDec = manualTiltDec + 1;
        }
        
        if(ps2x.Analog(PSS_LY) > 225){ //if left stick Y value is more than 155 (i.e moved down) then set tilt direction to down and step once)
          digitalWrite(tiltDir, HIGH);
          digitalWrite(tiltStep, HIGH);
          delay(1);
          digitalWrite(tiltStep, LOW);  
       // manualTiltInc = manualTiltInc + 1;
        }        
  
        if(ps2x.Analog(PSS_RX) < 30){ //if right stick Y value is less than 100 (i.e moved left) then set slide direction to left and step once)
          digitalWrite(slideDir, LOW);
          digitalWrite(slideStep, HIGH);
          delay(1);
          digitalWrite(slideStep, LOW);
       // manualSlideDec = manualSlideDec +1;
        }
        
        if(ps2x.Analog(PSS_RX) > 225){ //if right stick Y value is more than 155 (i.e moved right) then set slide direction to right and step once)
          digitalWrite(slideDir, HIGH);
          digitalWrite(slideStep, HIGH);
          delay(1);
          digitalWrite(slideStep, LOW);  
       // manualSlideInc = manualSlideInc + 1;
        }
  
        if(ps2x.Analog(PSS_RY) < 30){ //if right stick X value is less than 100 (i.e moved up) then set roll direction to anticlockwise and step once)
          digitalWrite(rollDir, LOW);
          digitalWrite(rollStep, HIGH);
          delay(1);
          digitalWrite(rollStep, LOW);
       // manualRollDec = manualRollDec + 1;
        }
        
        if(ps2x.Analog(PSS_RY) > 225){ //if right stick X value is more than 155 (i.e moved down) then set roll direction to clockwise and step once)
          digitalWrite(rollDir, HIGH);
          digitalWrite(rollStep, HIGH);
          delay(1);
          digitalWrite(rollStep, LOW);  
       // manualRollInc = manualRollInc + 1;
        }      
        
        if(ps2x.ButtonPressed(PSB_GREEN)){ //if the Green Triangle button is pressed then we change the manualExit variable which will cause us to exit the Manual Control Mode while statement and thus exit Manual Control Mode
          lcd.clear();  
          lcd.print("Exiting to Menu");
          delay(1000);
          manualExit = 1;
          manualDisplay = 0; //clears the variable that specifies whether to display the stick X/Y values or steps taken on the LCD
          manualPanInc = 0;  //resets step counters to 0 in preperation of entering MCM again
          manualPanDec = 0; 
          manualTiltInc = 0;
          manualTiltDec = 0;
          manualRollInc = 0;
          manualRollDec = 0;
          manualSlideInc = 0;
          manualSlideDec = 0;
          digitalWrite(panStep,0); //sets all pan motor control pins to 0 so that whether we enter the MCM or the ACS we do so with a clean state
          digitalWrite(panDir,0);
          digitalWrite(panRes1,0);
          digitalWrite(panRes2,0);
          digitalWrite(panRes3,0);
          digitalWrite(tiltStep,0); //sets all tilt motor control pins to 0 so that whether we enter the MCM or the ACS we do so with a clean state
          digitalWrite(tiltDir,0);
          digitalWrite(tiltRes1,0);
          digitalWrite(tiltRes2,0);
          digitalWrite(tiltRes3,0);
          digitalWrite(rollStep,0); //sets all roll motor control pins to 0 so that whether we enter the MCM or the ACS we do so with a clean state
          digitalWrite(rollDir,0);
          digitalWrite(rollRes1,0);
          digitalWrite(rollRes2,0);
          digitalWrite(rollRes3,0);
          digitalWrite(slideStep,0); //sets all slide motor control pins to 0 so that whether we enter the MCM or the ACS we do so with a clean state
          digitalWrite(slideDir,0);
          digitalWrite(slideRes1,0);
          digitalWrite(slideRes2,0);
          digitalWrite(slideRes3,0);
        }
        
      } //end of manual mode while statement   
      
  } // end of manual control mode selected if statement
  
  
  
  // AUTOMATIC CONTROL SETUP STARTS HERE
  
   /*program now checks to see if Automatic Control Setup was selected in the Main Menu (menuSelect == 2). 
   At this point in the program menuSelect can only either be 2 or 0. 
   It will be 2 if Automatic Control Setup was selected in the Main Menu or 0 if the Manual Control Mode was selected and then exited.
   The first thing that happens when entering Manual Control Mode is that the menuSelect int is set to 0 to return us to the Main Menu when Manual Control Mode has been exited */
   
   if(menuSelect == 2){ //start of if statement that is true if Automatic Control Setup was selected in the Main Menu
    menuSelect = 0; //this will ensure that we are returned to the start of the loop() function and not dumped into Manual Control Mode when the Automatic Control Setup is exited
    lcd.clear();
    
    autoMenuNext = 0; //prevents us from moving onto the next menu item unless the X button has been pressed to confirm settings
               
                lcd.setCursor (0,0); 
                lcd.print ("Program Duration =");
                lcd.setCursor (4,1); 
                lcd.print ("Minutes"); 
                lcd.setCursor (0,2);
                lcd.print ("or");
                lcd.setCursor(6,3);
                lcd.print ("Hours");
                autoRunTime = 0;
                autoRunTimeHours = 0;
                
    while(autoMenuNext == 0){ //Begining of while statement setting Program Duration
                ps2x.read_gamepad();           
                
                lcd.setCursor (0,1);
                lcd.print (autoRunTime);

                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Increase Program Duration by 1 minute
                  autoRunTime = autoRunTime + 1;
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Increase Program Duration by 1 minute
                  autoRunTime = autoRunTime - 1;
                }

                if(ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Increase Program Duration by 1 minute
                  autoRunTime = autoRunTime - 60;
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) { //Increase Program Duration by 1 minute
                  autoRunTime = autoRunTime + 60;
                }                
                
                if(autoRunTime < 0){
                  autoRunTime = 0;
                }
                                
                if(autoRunTime < 10){ //if the number is less than 10 then cover any spaces used when the number is higher than 10 with a space so that the number displayed is correct
                  lcd.setCursor(1,1);
                  lcd.print("   ");
                }
                
                else if(autoRunTime < 100){ //if the number is less than 100 then cover any spaces used when the number is higher than 100 with a space so that the number displayed is correct
                  lcd.setCursor(2,1);
                  lcd.print("  ");
                }
                
                else if(autoRunTime < 1000){ //if the number is less than 1000 then cover any spaces used when the number is higher than 1000 with a space so that the number displayed is correct
                  lcd.setCursor(3,1);
                  lcd.print(" ");
                }
                          
                autoRunTimeHours = (float) autoRunTime / 60.0; //calculation to display time as hours
                lcd.setCursor (0,3);
                lcd.print (autoRunTimeHours);
                delay(50); //delay used to set how quickly the numbers change. use this to adjust sensitivity to button presses etc
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 1;
                  autoRunTimeMS = autoRunTime * 60000;
                }
                
  } // End of while statement setting Program Duration 
  
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Pan Enable = "); 
                lcd.setCursor (0,2); 
                lcd.print ("Up = Yes / Down = No");
                lcd.setCursor(0,1);
                lcd.print("No ");
                
    while(autoMenuNext == 1){ //Begining of while statement setting Pan Enable / Disable
                ps2x.read_gamepad();                
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Set autoPanEnable to Yes
                  autoPanEnable = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("Yes");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Set autoPanEnable to No
                  autoPanEnable = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("No ");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  if(autoPanEnable == 0){
                    autoMenuNext = 5;
                  }
                  else autoMenuNext = 2;
                }
         
  } // End of while statement setting Pan Enable / Disable 
                
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Pan Direction =");
                lcd.setCursor (0,2); 
                lcd.print ("Left Arrow = Left");
                lcd.setCursor (0,3);
                lcd.print ("Right Arrow = Right"); 
                lcd.setCursor(0,1);
                lcd.print("Left");

    while(autoMenuNext == 2){ //Begining of while statement setting Pan Direction
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Set autoPanDir to Left
                  autoPanDir = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);                  
                  lcd.print ("Left ");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) { //Set autoPanDir to Right
                  autoPanDir = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);
                  lcd.print ("Right");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 3;
                }
  } // End of while statement setting Pan Direction 

                lcd.clear();
                lcd.setCursor (0,0); 
                lcd.print ("Pan Interval =");
                lcd.setCursor (5,1);
                lcd.print ("Seconds");
                lcd.setCursor (0,1);
                lcd.print ("1.0"); //print default value of 1.0 Seconds
                
    while(autoMenuNext == 3){ //Begining of while statement setting Pan Interval
                ps2x.read_gamepad();
                              
                if(ps2x.ButtonPressed(PSB_PAD_UP)){ //if the Up button is pressed then the Interval value is increased by a value of 1 
                  autoPanIntSet = autoPanIntSet + 1;
                         
                          if(autoPanIntSet == 8){ //if the Interval value is 8 then the value wraps around to equal 2
                             autoPanIntSet = 2;
                          }
              
                  lcd.setCursor(0,1); 
                  lcd.print ("    ");
                  
                  if(autoPanIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoPanIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoPanIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoPanIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoPanIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoPanIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  }
              }
                
              if(ps2x.ButtonPressed(PSB_PAD_DOWN)){ //if the Down button is pressed then the Interval value is decreased by a value of 1 
                  autoPanIntSet = autoPanIntSet - 1;
                        
                          if(autoPanIntSet == 1){ //if the Interval value is 1 then the value wraps around to equal 9
                             autoPanIntSet = 7;
                          }
               
                  lcd.setCursor(0,1);
                  lcd.print("    "); 
                  
                  if(autoPanIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoPanIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoPanIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoPanIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoPanIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoPanIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  } 
                }
                
            if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
              autoMenuNext = 4;
                  
                  if(autoPanIntSet == 2){
                    autoPanInt = 1000;
                  }
                  
                  else if(autoPanIntSet == 3){
                    autoPanInt = 2000;
                  }
                  
                  else if(autoPanIntSet == 4){
                    autoPanInt = 5000;
                  }
                  
                  else if(autoPanIntSet == 5){
                    autoPanInt = 10000;
                  }
                  
                  else if(autoPanIntSet == 6){
                    autoPanInt = 30000;
                  }
                  
                  else if(autoPanIntSet == 7){
                    autoPanInt = 60000;
                  }              
              
            }
  } // End of while statement setting Pan Interval    
 
 
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Pan Resolution =");
                lcd.setCursor (0,1);
                lcd.print ("Full Step Mode"); 

    while(autoMenuNext == 4){ //Begining of while statement setting Pan Resolution
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Scroll Up Through Resolutions
                  autoPanRes = autoPanRes + 1;
                
                    if(autoPanRes == 7){
                      autoPanRes = 2;
                    }
                    
                    if(autoPanRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoPanRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoPanRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoPanRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoPanRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                    
                }

                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Scroll Down Through Resolutions
                  autoPanRes = autoPanRes - 1;
                
                    if(autoPanRes == 1){
                      autoPanRes = 6;
                    }
                    
                    if(autoPanRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoPanRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoPanRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoPanRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoPanRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                                 
                }
                        
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 5;
                }
                
  } // End of while statement setting Pan Resolution 

                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Tilt Enable = "); 
                lcd.setCursor (0,2); 
                lcd.print ("Up = Yes / Down = No");
                lcd.setCursor (0,1);
                lcd.print("No ");
                
    while(autoMenuNext == 5){ //Begining of while statement setting Tilt Enable / Disable
                ps2x.read_gamepad();                
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Set autoTiltEnable to Yes
                  autoTiltEnable = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("Yes");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Set autoTiltEnable to No
                  autoTiltEnable = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("No ");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  if(autoTiltEnable == 0){
                    autoMenuNext = 9;
                  }
                  else autoMenuNext = 6;
                }
         
  } // End of while statement setting Tilt Enable / Disable 
                
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Tilt Direction =");
                lcd.setCursor (0,2); 
                lcd.print ("Up Arrow = Up");
                lcd.setCursor (0,3);
                lcd.print ("Down Arrow = Down"); 
                lcd.setCursor (0,1);
                lcd.print("Down");
                
    while(autoMenuNext == 6){ //Begining of while statement setting Tilt Direction
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Set autoTiltDir to Up
                  autoTiltDir = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);                  
                  lcd.print ("Up  ");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Set autoTiltDir to Down
                  autoTiltDir = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);
                  lcd.print ("Down");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 7;
                }
  } // End of while statement setting Tilt Direction 

                lcd.clear();
                lcd.setCursor (0,0); 
                lcd.print ("Tilt Interval =");
                lcd.setCursor (5,1);
                lcd.print ("Seconds");
                lcd.setCursor (0,1);
                lcd.print ("1.0"); //print default value of 1.0 Seconds
                
    while(autoMenuNext == 7){ //Begining of while statement setting Tilt Interval
                ps2x.read_gamepad();
                              
                if(ps2x.ButtonPressed(PSB_PAD_UP)){ //if the Up button is pressed then the Interval value is increased by a value of 1 
                  autoTiltIntSet = autoTiltIntSet + 1;
                         
                          if(autoTiltIntSet == 8){ //if the Interval value is 8 then the value wraps around to equal 2
                             autoTiltIntSet = 2;
                          }
              
                  lcd.setCursor(0,1); 
                  lcd.print ("    ");
                  
                  if(autoTiltIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoTiltIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoTiltIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoTiltIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoTiltIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoTiltIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  }
              }
                
              if(ps2x.ButtonPressed(PSB_PAD_DOWN)){ //if the Down button is pressed then the Interval value is decreased by a value of 1 
                  autoTiltIntSet = autoTiltIntSet - 1;
                        
                          if(autoTiltIntSet == 1){ //if the Interval value is 1 then the value wraps around to equal 9
                             autoTiltIntSet = 7;
                          }
               
                  lcd.setCursor(0,1);
                  lcd.print("    "); 
                  
                  if(autoTiltIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoTiltIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoTiltIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoTiltIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoTiltIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoTiltIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  } 
                }
                
            if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
              autoMenuNext = 8;
                  
                  if(autoTiltIntSet == 2){
                    autoTiltInt = 1000;
                  }
                  
                  else if(autoTiltIntSet == 3){
                    autoTiltInt = 2000;
                  }
                  
                  else if(autoTiltIntSet == 4){
                    autoTiltInt = 5000;
                  }
                  
                  else if(autoTiltIntSet == 5){
                    autoTiltInt = 10000;
                  }
                  
                  else if(autoTiltIntSet == 6){
                    autoTiltInt = 30000;
                  }
                  
                  else if(autoTiltIntSet == 7){
                    autoTiltInt = 60000;
                  }              
              
            }
  } // End of while statement setting Tilt Interval    
 
 
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Tilt Resolution =");
                lcd.setCursor (0,1);
                lcd.print ("Full Step Mode"); 

    while(autoMenuNext == 8){ //Begining of while statement setting Tilt Resolution
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Scroll Up Through Resolutions
                  autoTiltRes = autoTiltRes + 1;
                
                    if(autoTiltRes == 7){
                      autoTiltRes = 2;
                    }
                    
                    if(autoTiltRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoTiltRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoTiltRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoTiltRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoTiltRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                    
                }

                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Scroll Down Through Resolutions
                  autoTiltRes = autoTiltRes - 1;
                
                    if(autoTiltRes == 1){
                      autoTiltRes = 6;
                    }
                    
                    if(autoTiltRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoTiltRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoTiltRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoTiltRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoTiltRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                                 
                }
                        
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 9;
                }
            }

                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Roll Enable = "); 
                lcd.setCursor (0,2); 
                lcd.print ("Up = Yes / Down = No");
                lcd.setCursor (0,1);
                lcd.print("No ");
                
    while(autoMenuNext == 9){ //Begining of while statement setting Roll Enable / Disable
                ps2x.read_gamepad();                
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Set autoRollEnable to Yes
                  autoRollEnable = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("Yes");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Set autoRollEnable to No
                  autoRollEnable = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("No ");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  if(autoRollEnable == 0){
                    autoMenuNext = 13;
                  }
                  else autoMenuNext = 10;
                }
         
  } // End of while statement setting Roll Enable / Disable 
                
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Roll Direction =");
                lcd.setCursor (0,2); 
                lcd.print ("Left Arrow = CCW");
                lcd.setCursor (0,3);
                lcd.print ("Right Arrow = CW"); 
                lcd.setCursor (0,1);
                lcd.print("CW");
                
    while(autoMenuNext == 10){ //Begining of while statement setting Roll Direction
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Set autoRollDir to CCW
                  autoRollDir = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);                  
                  lcd.print ("CCW");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) { //Set autoRollDir to CW
                  autoRollDir = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);
                  lcd.print ("CW ");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 11;
                }
  } // End of while statement setting Roll Direction 

                lcd.clear();
                lcd.setCursor (0,0); 
                lcd.print ("Roll Interval =");
                lcd.setCursor (5,1);
                lcd.print ("Seconds");
                lcd.setCursor (0,1);
                lcd.print ("1.0"); //print default value of 1.0 Seconds
                
    while(autoMenuNext == 11){ //Begining of while statement setting Roll Interval
                ps2x.read_gamepad();
                              
                if(ps2x.ButtonPressed(PSB_PAD_UP)){ //if the Up button is pressed then the Interval value is increased by a value of 1 
                  autoRollIntSet = autoRollIntSet + 1;
                         
                          if(autoRollIntSet == 8){ //if the Interval value is 8 then the value wraps around to equal 2
                             autoRollIntSet = 2;
                          }
              
                  lcd.setCursor(0,1); 
                  lcd.print ("    ");
                  
                  if(autoRollIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoRollIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoRollIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoRollIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoRollIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoRollIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  }
              }
                
              if(ps2x.ButtonPressed(PSB_PAD_DOWN)){ //if the Down button is pressed then the Interval value is decreased by a value of 1 
                  autoRollIntSet = autoRollIntSet - 1;
                        
                          if(autoRollIntSet == 1){ //if the Interval value is 1 then the value wraps around to equal 9
                             autoRollIntSet = 7;
                          }
               
                  lcd.setCursor(0,1);
                  lcd.print("    "); 
                  
                  if(autoRollIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoRollIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoRollIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoRollIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoRollIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoRollIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  } 
                }
                
            if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
              autoMenuNext = 12;
                  
                  if(autoRollIntSet == 2){
                    autoRollInt = 1000;
                  }
                  
                  else if(autoRollIntSet == 3){
                    autoRollInt = 2000;
                  }
                  
                  else if(autoRollIntSet == 4){
                    autoRollInt = 5000;
                  }
                  
                  else if(autoRollIntSet == 5){
                    autoRollInt = 10000;
                  }
                  
                  else if(autoRollIntSet == 6){
                    autoRollInt = 30000;
                  }
                  
                  else if(autoRollIntSet == 7){
                    autoRollInt = 60000;
                  }              
              
            }
  } // End of while statement setting Roll Interval    
 
 
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Roll Resolution =");
                lcd.setCursor (0,1);
                lcd.print ("Full Step Mode"); 

    while(autoMenuNext == 12){ //Begining of while statement setting Roll Resolution
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Scroll Up Through Resolutions
                  autoRollRes = autoRollRes + 1;
                
                    if(autoRollRes == 7){
                      autoRollRes = 2;
                    }
                    
                    if(autoRollRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoRollRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoRollRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoRollRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoRollRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                    
                }

                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Scroll Down Through Resolutions
                  autoRollRes = autoRollRes - 1;
                
                    if(autoRollRes == 1){
                      autoRollRes = 6;
                    }
                    
                    if(autoRollRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoRollRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoRollRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoRollRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoRollRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                                 
                }
                        
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 13;
                }
            }

                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Slide Enable = "); 
                lcd.setCursor (0,2); 
                lcd.print ("Up = Yes / Down = No");
                lcd.setCursor (0,1);
                lcd.print("No ");
                
    while(autoMenuNext == 13){ //Begining of while statement setting Slide Enable / Disable
                ps2x.read_gamepad();                
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Set autoSlideEnable to Yes
                  autoSlideEnable = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("Yes");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Set autoSlideEnable to No
                  autoSlideEnable = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("No ");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  if(autoSlideEnable == 0){
                    autoMenuNext = 17;
                  }
                  else autoMenuNext = 14;
                }
         
  } // End of while statement setting Slide Enable / Disable 
                
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Slide Direction =");
                lcd.setCursor (0,2); 
                lcd.print ("Left Arrow = Left");
                lcd.setCursor (0,3);
                lcd.print ("Right Arrow = Right"); 
                lcd.setCursor (0,1);
                lcd.print("Left");
                
    while(autoMenuNext == 14){ //Begining of while statement setting Slide Direction
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_LEFT)) { //Set autoSlideDir to Left
                  autoSlideDir = 0;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);                  
                  lcd.print ("Left ");
                }
                
                if(ps2x.ButtonPressed(PSB_PAD_RIGHT)) { //Set autoSlideDir to Right
                  autoSlideDir = 1;
                  lcd.setCursor (0,1);
                  lcd.print ("     ");
                  lcd.setCursor (0,1);
                  lcd.print ("Right");     
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 15;
                }
  } // End of while statement setting Slide Direction 

                lcd.clear();
                lcd.setCursor (0,0); 
                lcd.print ("Slide Interval =");
                lcd.setCursor (5,1);
                lcd.print ("Seconds");
                lcd.setCursor (0,1);
                lcd.print ("1.0"); //print default value of 1.0 Seconds
                
    while(autoMenuNext == 15){ //Begining of while statement setting Slide Interval
                ps2x.read_gamepad();
                              
                if(ps2x.ButtonPressed(PSB_PAD_UP)){ //if the Up button is pressed then the Interval value is increased by a value of 1 
                  autoSlideIntSet = autoSlideIntSet + 1;
                         
                          if(autoSlideIntSet == 8){ //if the Interval value is 8 then the value wraps around to equal 2
                             autoSlideIntSet = 2;
                          }
              
                  lcd.setCursor(0,1); 
                  lcd.print ("    ");
                  
                  if(autoSlideIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoSlideIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoSlideIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoSlideIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoSlideIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoSlideIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  }
              }
                
              if(ps2x.ButtonPressed(PSB_PAD_DOWN)){ //if the Down button is pressed then the Interval value is decreased by a value of 1 
                  autoSlideIntSet = autoSlideIntSet - 1;
                        
                          if(autoSlideIntSet == 1){ //if the Interval value is 1 then the value wraps around to equal 9
                             autoSlideIntSet = 7;
                          }
               
                  lcd.setCursor(0,1);
                  lcd.print("    "); 
                  
                  if(autoSlideIntSet == 2){
                    lcd.setCursor(0,1);
                    lcd.print("1.0 ");
                  }
                  
                  else if(autoSlideIntSet == 3){
                    lcd.setCursor(0,1);
                    lcd.print("2.0 ");
                  }
                  
                  else if(autoSlideIntSet == 4){
                    lcd.setCursor(0,1);
                    lcd.print("5.0 ");
                  }
                  
                  else if(autoSlideIntSet == 5){
                    lcd.setCursor(0,1);
                    lcd.print("10.0");
                  }
                  
                  else if(autoSlideIntSet == 6){
                    lcd.setCursor(0,1);
                    lcd.print("30.0");
                  }
                  
                  else if(autoSlideIntSet == 7){
                    lcd.setCursor(0,1);
                    lcd.print("60.0");
                  } 
                }
                
            if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
              autoMenuNext = 16;
                  
                  if(autoSlideIntSet == 2){
                    autoSlideInt = 1000;
                  }
                  
                  else if(autoSlideIntSet == 3){
                    autoSlideInt = 2000;
                  }
                  
                  else if(autoSlideIntSet == 4){
                    autoSlideInt = 5000;
                  }
                  
                  else if(autoSlideIntSet == 5){
                    autoSlideInt = 10000;
                  }
                  
                  else if(autoSlideIntSet == 6){
                    autoSlideInt = 30000;
                  }
                  
                  else if(autoSlideIntSet == 7){
                    autoSlideInt = 60000;
                  }              
              
            }
  } // End of while statement setting Slide Interval    
 
 
                lcd.clear();
                lcd.setCursor (0,0);
                lcd.print ("Slide Resolution =");
                lcd.setCursor (0,1);
                lcd.print ("Full Step Mode"); 

    while(autoMenuNext == 16){ //Begining of while statement setting Slide Resolution
                ps2x.read_gamepad();
                
                if(ps2x.ButtonPressed(PSB_PAD_UP)) { //Scroll Up Through Resolutions
                  autoSlideRes = autoSlideRes + 1;
                
                    if(autoSlideRes == 7){
                      autoSlideRes = 2;
                    }
                    
                    if(autoSlideRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoSlideRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoSlideRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoSlideRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoSlideRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                    
                }

                if(ps2x.ButtonPressed(PSB_PAD_DOWN)) { //Scroll Down Through Resolutions
                  autoSlideRes = autoSlideRes - 1;
                
                    if(autoSlideRes == 1){
                      autoSlideRes = 6;
                    }
                    
                    if(autoSlideRes == 2){
                      lcd.setCursor(0,1);
                      lcd.print("Full Step Mode      ");
                    }
                    
                    else if(autoSlideRes == 3){
                      lcd.setCursor(0,1);                      
                      lcd.print("Half Step Mode      ");
                    }
                    
                    else if(autoSlideRes == 4){
                      lcd.setCursor(0,1);                      
                      lcd.print("Quarter Step Mode   ");
                    }
                    
                    else if(autoSlideRes == 5){
                      lcd.setCursor(0,1);
                      lcd.print("Eighth Step Mode    ");
                    }
                    
                    else if(autoSlideRes == 6){
                      lcd.setCursor(0,1);
                      lcd.print("Sixteenth Step Mode ");
                    }                                 
                }
                        
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 17;
                }
            }

 
                lcd.clear();
    while(autoMenuNext == 17){ //Begining of while statement displaying ACS settings
                ps2x.read_gamepad();
                
                if(autoPanEnable == 1){ //Pan Sum
                  lcd.setCursor (0,0);
                  lcd.print("P-EN");
                  
                  if(autoPanDir == 0){
                    lcd.setCursor (0,1);
                    lcd.print("LFT");
                  }
                  if(autoPanDir == 1){
                    lcd.setCursor(0,1);
                    lcd.print("RGT");
                  }
                  
                  lcd.setCursor(0,2);
                  lcd.print(autoPanInt / 1000);
                  
                  if(autoPanRes == 2){
                    lcd.setCursor(0,3);
                    lcd.print("FWS");
                  }
                  else if(autoPanRes == 3){
                    lcd.setCursor(0,3);
                    lcd.print("HWS");
                  }
                  else if(autoPanRes == 4){
                    lcd.setCursor(0,3);
                    lcd.print("QWS");
                  }
                  else if(autoPanRes == 5){
                    lcd.setCursor(0,3);
                    lcd.print("EWS");
                  }
                  else if(autoPanRes == 6){
                    lcd.setCursor(0,3);
                    lcd.print("SWS");
                  }    
                }
                
                
                  if(autoTiltEnable == 1){ //Tilt Sum
                  lcd.setCursor (5,0);
                  lcd.print("T-EN");
                  
                  if(autoTiltDir == 0){
                    lcd.setCursor (5,1);
                    lcd.print("DWN");
                  }
                  if(autoTiltDir == 1){
                    lcd.setCursor(5,1);
                    lcd.print("UP");
                  }
                  
                  lcd.setCursor(5,2);
                  lcd.print(autoTiltInt / 1000);
                  
                  if(autoTiltRes == 2){
                    lcd.setCursor(5,3);
                    lcd.print("FWS");
                  }
                  else if(autoTiltRes == 3){
                    lcd.setCursor(5,3);
                    lcd.print("HWS");
                  }
                  else if(autoTiltRes == 4){
                    lcd.setCursor(5,3);
                    lcd.print("QWS");
                  }
                  else if(autoTiltRes == 5){
                    lcd.setCursor(5,3);
                    lcd.print("EWS");
                  }
                  else if(autoTiltRes == 6){
                    lcd.setCursor(5,3);
                    lcd.print("SWS");
                  }    
                }
    

                  if(autoRollEnable == 1){ //Roll Sum
                  lcd.setCursor (10,0);
                  lcd.print("R-EN");
                  
                  if(autoRollDir == 0){
                    lcd.setCursor (10,1);
                    lcd.print("CW");
                  }
                  if(autoRollDir == 1){
                    lcd.setCursor(10,1);
                    lcd.print("CCW");
                  }
                  
                  lcd.setCursor(10,2);
                  lcd.print(autoRollInt / 1000);
                  
                  if(autoRollRes == 2){
                    lcd.setCursor(10,3);
                    lcd.print("FWS");
                  }
                  else if(autoRollRes == 3){
                    lcd.setCursor(10,3);
                    lcd.print("HWS");
                  }
                  else if(autoRollRes == 4){
                    lcd.setCursor(10,3);
                    lcd.print("QWS");
                  }
                  else if(autoRollRes == 5){
                    lcd.setCursor(10,3);
                    lcd.print("EWS");
                  }
                  else if(autoRollRes == 6){
                    lcd.setCursor(10,3);
                    lcd.print("SWS");
                  }    
                }    
                   
                                      
                  if(autoSlideEnable == 1){ //Slide Sum
                  lcd.setCursor (15,0);
                  lcd.print("S-EN");
                  
                  if(autoSlideDir == 0){
                    lcd.setCursor (15,1);
                    lcd.print("LFT");
                  }
                  if(autoSlideDir == 1){
                    lcd.setCursor(15,1);
                    lcd.print("RGT");
                  }
                  
                  lcd.setCursor(15,2);
                  lcd.print(autoSlideInt / 1000);
                  
                  if(autoSlideRes == 2){
                    lcd.setCursor(15,3);
                    lcd.print("FWS");
                  }
                  else if(autoSlideRes == 3){
                    lcd.setCursor(15,3);
                    lcd.print("HWS");
                  }
                  else if(autoSlideRes == 4){
                    lcd.setCursor(15,3);
                    lcd.print("QWS");
                  }
                  else if(autoSlideRes == 5){
                    lcd.setCursor(15,3);
                    lcd.print("EWS");
                  }
                  else if(autoSlideRes == 6){
                    lcd.setCursor(15,3);
                    lcd.print("SWS");
                  }    
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 18;
                }
            } // end of while statement displaying ACS settings
            
    lcd.clear();
    while(autoMenuNext == 18){ //Begining of while statement displaying ACS settings and setting direction and resolution pin output states. Setting the pin states for these here saves us having to do them in the next section.
                ps2x.read_gamepad();
                
                lcd.setCursor(3,1);
                lcd.print("Press X to Run");
                
                  if(autoPanEnable == 1){ //Pan Direction and Resolution pin settings
                  
                  if(autoPanDir == 0){
                    digitalWrite(panDir,LOW); //change this if the pan motor runs the wrong way
                  }
                  
                  if(autoPanDir == 1){
                    digitalWrite(panDir,HIGH); //change this if the pan motor runs the wrong way
                  }
                                  
                  if(autoPanRes == 2){
                    digitalWrite(panRes1,LOW);
                    digitalWrite(panRes2,LOW);
                    digitalWrite(panRes3,LOW);
                  
                  }
                  else if(autoPanRes == 3){
                    digitalWrite(panRes1,HIGH);
                    digitalWrite(panRes2,LOW);
                    digitalWrite(panRes3,LOW);
                  }
                  else if(autoPanRes == 4){
                    digitalWrite(panRes1,LOW);
                    digitalWrite(panRes2,HIGH);
                    digitalWrite(panRes3,LOW);
                  }
                  else if(autoPanRes == 5){
                    digitalWrite(panRes1,HIGH);
                    digitalWrite(panRes2,HIGH);
                    digitalWrite(panRes3,LOW);
                  }
                  else if(autoPanRes == 6){
                    digitalWrite(panRes1,HIGH);
                    digitalWrite(panRes2,HIGH);
                    digitalWrite(panRes3,HIGH);
                  }    
                }
                
                
                  if(autoTiltEnable == 1){ //Tilt Direction and Resolution pin settings
                  
                  if(autoTiltDir == 0){
                    digitalWrite(tiltDir,LOW); //change this if the tilt motor runs the wrong way
                  }
                  
                  if(autoTiltDir == 1){
                    digitalWrite(tiltDir,HIGH); //change this if the tilt motor runs the wrong way
                  }
                  
                  if(autoTiltRes == 2){
                    digitalWrite(tiltRes1,LOW);
                    digitalWrite(tiltRes2,LOW);
                    digitalWrite(tiltRes3,LOW);
                  }
                  else if(autoTiltRes == 3){
                    digitalWrite(tiltRes1,HIGH);
                    digitalWrite(tiltRes2,LOW);
                    digitalWrite(tiltRes3,LOW);
                  }
                  else if(autoTiltRes == 4){
                    digitalWrite(tiltRes1,LOW);
                    digitalWrite(tiltRes2,HIGH);
                    digitalWrite(tiltRes3,LOW);
                  }
                  else if(autoTiltRes == 5){
                    digitalWrite(tiltRes1,HIGH);
                    digitalWrite(tiltRes2,HIGH);
                    digitalWrite(tiltRes3,LOW);
                  }
                  else if(autoTiltRes == 6){
                    digitalWrite(tiltRes1,HIGH);
                    digitalWrite(tiltRes2,HIGH);
                    digitalWrite(tiltRes3,HIGH);
                  }    
                }
    

                  if(autoRollEnable == 1){ //Roll Direction and Resolution pin settings
                  
                  if(autoRollDir == 0){
                    digitalWrite(rollDir,LOW); //change this if the roll motor runs the wrong way
                  }
                  
                  if(autoRollDir == 1){
                    digitalWrite(rollDir,HIGH); //change this if the roll motor runs the wrong way
                  }
                  
                  if(autoRollRes == 2){
                    digitalWrite(rollRes1,LOW);
                    digitalWrite(rollRes2,LOW);
                    digitalWrite(rollRes3,LOW);
                  }
                  else if(autoRollRes == 3){
                    digitalWrite(rollRes1,HIGH);
                    digitalWrite(rollRes2,LOW);
                    digitalWrite(rollRes3,LOW);
                  }
                  else if(autoRollRes == 4){
                    digitalWrite(rollRes1,LOW);
                    digitalWrite(rollRes2,HIGH);
                    digitalWrite(rollRes3,LOW);
                  }
                  else if(autoRollRes == 5){
                    digitalWrite(rollRes1,HIGH);
                    digitalWrite(rollRes2,HIGH);
                    digitalWrite(rollRes3,LOW);
                  }
                  else if(autoRollRes == 6){
                    digitalWrite(rollRes1,HIGH);
                    digitalWrite(rollRes2,HIGH);
                    digitalWrite(rollRes3,HIGH);
                  }    
                }    
                   
                                      
                  if(autoSlideEnable == 1){ //Slide Direction and Resolution pin settings
                  
                  if(autoSlideDir == 0){
                    digitalWrite(slideDir,LOW); //change this if the slide motor runs the wrong way
                  }
                  
                  if(autoSlideDir == 1){
                    digitalWrite(slideDir,HIGH); //change this if the slide motor runs the wrong way
                  }
                  
                  if(autoSlideRes == 2){
                    digitalWrite(slideRes1,LOW);
                    digitalWrite(slideRes2,LOW);
                    digitalWrite(slideRes3,LOW);
                  }
                  else if(autoSlideRes == 3){
                    digitalWrite(slideRes1,HIGH);
                    digitalWrite(slideRes2,LOW);
                    digitalWrite(slideRes3,LOW);
                  }
                  else if(autoSlideRes == 4){
                    digitalWrite(slideRes1,LOW);
                    digitalWrite(slideRes2,HIGH);
                    digitalWrite(slideRes3,LOW);
                  }
                  else if(autoSlideRes == 5){
                    digitalWrite(slideRes1,HIGH);
                    digitalWrite(slideRes2,HIGH);
                    digitalWrite(slideRes3,LOW);
                  }
                  else if(autoSlideRes == 6){
                    digitalWrite(slideRes1,HIGH);
                    digitalWrite(slideRes2,HIGH);
                    digitalWrite(slideRes3,HIGH);
                  }    
                }
                
                if(ps2x.ButtonPressed(PSB_BLUE)){ //if the Blue X button is pressed then we change the autoMenuNext variable which will cause us to move to the next menu section
                  autoMenuNext = 19;
                }
          }// end of "Press X to run" screen

    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print(autoRunTime); //displays the time in Minutes that the program WILL run for        
    lcd.print(" Minutes");
    lcd.setCursor(0,1);
    lcd.print(autoRunTimeHours);
    lcd.print(" Hours");   
       
    autoRunStart = millis(); //stores the time that the program started in MS (place this as close to the start of the while loop as possible)   
    autoPanNext = millis() - autoRunStart;
    autoTiltNext = millis() - autoRunStart;
    autoRollNext = millis() - autoRunStart;
    autoSlideNext = millis() - autoRunStart;
    
    if(autoPanEnable == 0){ //if autoPanEnable == 0 then we set the time until the next pan step to be the max of 1100 odd hours into the program. This effectivly disables movement in this axis
      autoPanNext = 4294967294;
    }
    
    if(autoTiltEnable == 0){
      autoTiltNext = 4294967294;
    }
    
    if(autoRollEnable == 0){
      autoRollNext = 4294967294;
    }
  
    if(autoSlideEnable == 0){
      autoSlideNext = 4294967294;
    }    

    while(autoMenuNext == 19){ //Begining of while statement which runs the ARP set up in the ACS
                ps2x.read_gamepad();
                                             
                autoRunTimeClock = millis() - autoRunStart; //calcualtes the time in MS seconds that the program has run for                
                lcd.setCursor(0,2);
                lcd.print(autoRunTimeClock / 1000); //displays the time in Seconds that the program has left to run
                lcd.print(" Sec");
                lcd.setCursor(0,3);
                lcd.print(autoRunTimeClock / 60000);
                lcd.print(" Min");
                
                if(autoRunTimeClock > autoPanNext){ // Checks if enough time has passed between the last step to pan the motor and the next step
                 digitalWrite(panStep,HIGH); 
                 delay(1);                   
                 digitalWrite(panStep,LOW);
                 autoPanPrev = autoPanNext; //change the step pulse that just happened from being the next step pulse to the last
                 autoPanNext = autoPanPrev + autoPanInt; //log the time for the next pan step pulse to be output 
                }

                if(autoRunTimeClock > autoTiltNext){ // Checks if enough time has passed between the last step to tilt the motor and the next step
                 digitalWrite(tiltStep,HIGH); 
                 delay(1);                   
                 digitalWrite(tiltStep,LOW);
                 autoTiltPrev = autoTiltNext; //change the step pulse that just happened from being the next step pulse to the last
                 autoTiltNext = autoTiltPrev + autoTiltInt; //log the time for the next tilt step pulse to be output 
                }
                
                
                if(autoRunTimeClock > autoRollNext){ // Checks if enough time has passed between the last step to roll the motor and the next step
                 digitalWrite(rollStep,HIGH); 
                 delay(1);                   
                 digitalWrite(rollStep,LOW);
                 autoRollPrev = autoRollNext; //change the step pulse that just happened from being the next step pulse to the last
                 autoRollNext = autoRollPrev + autoRollInt; //log the time for the next pan step pulse to be output 
                }                
                
                if(autoRunTimeClock > autoSlideNext){ // Checks if enough time has passed between the last step to pan the motor and the next step
                 digitalWrite(slideStep,HIGH); 
                 delay(1);                   
                 digitalWrite(slideStep,LOW);
                 autoSlidePrev = autoSlideNext; //change the step pulse that just happened from being the next step pulse to the last
                 autoSlideNext = autoSlidePrev + autoSlideInt; //log the time for the next pan step pulse to be output 
                }
                
                if(autoRunTimeClock > autoRunTimeMS){ //if the Run Time Clock is greater than the time that the program is going to run for (autoRunTime) then we change the autoMenuNext variable and exit to the main menu
                  autoMenuNext = 20;
                }
                
                if(ps2x.ButtonPressed(PSB_GREEN)){ //if the Green Triangle button is pressed then we change the autoMenuNext variable and exit to the main menu
                  autoMenuNext = 20;
                }              
    }

        autoPanRes = 2;  //reset all the values used during the ACS and ARP so next time we enter with a clean slate
        autoTiltRes = 2;
        autoRollRes = 2;
        autoSlideRes = 2; 
        
        autoPanEnable = 0;
        autoTiltEnable = 0;
        autoRollEnable = 0;
        autoSlideEnable = 0;
        
        autoPanDir = 0;
        autoTiltDir = 0;
        autoRollDir = 0;
        autoSlideDir = 0;
        
        autoPanIntSet = 2;
        autoTiltIntSet = 2;
        autoRollIntSet = 2;
        autoSlideIntSet = 2;
        
        autoPanInt = 0; 
        autoTiltInt = 0;
        autoRollInt = 0;
        autoSlideInt = 0;
        
        autoRunTime = 0;
        autoRunTimeHours = 0;
        autoRunTimeMS = 0;
        autoMenuNext = 0;
        autoRunStart = 0;
        autoRunTimeClock = 0;
        autoPanNext = 0;
        autoTiltNext = 0;
        autoRollNext = 0;
        autoSlideNext = 0;
        autoPanPrev = 0;
        autoTiltPrev = 0;
        autoRollPrev = 0;
        autoSlidePrev = 0;
  
  
     } //end of ACS / ARP if statement 
   
}  //end of loop() function










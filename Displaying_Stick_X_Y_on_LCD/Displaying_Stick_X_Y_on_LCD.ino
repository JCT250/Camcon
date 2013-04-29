
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
int manualResolution = 2; //stores the stepping resolution when in manual control mode
int manualExit = 0; //Sets the value used to decide whether to exit the Manual Mode while statement

void setup(){
  Serial.begin(57600);

  lcd.begin(16, 2); // set up the LCD's number of columns and rows: 
 // lcd.print("Welcome to CAMCON"); // welcome message
  //delay(450); // initial delay before scrolling
  //for (int positionCounter = 0; positionCounter < 17; positionCounter++) { //(Sets initial value, argument to exit, increases value with each loop)
    //lcd.scrollDisplayLeft();
    //delay(450); //sets delay between character scrolling
  //}

  error = ps2x.config_gamepad(13,11,10,12, false, false);   //setup pins and settings:  GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error

  if(error == 0){
    Serial.println("Found Controller, configured successful");
    //Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    //Serial.println("holding L1 or R1 will print out the analog stick values.");
    //Serial.println("Go to www.billporter.info for updates and to report bugs.");
  }

  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");

  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");

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

}

void loop(){
  if(error == 1) //skip loop if no controller found
    return; 


  //MANUAL CONTROL STARTS HERE
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Manual Mode");
  lcd.setCursor(0,1);
  lcd.print("Triangle to Exit");
  delay(2000);
  lcd.clear();
  manualExit = 0;
  lcd.print("Full Step Mode"); //prints the stepping mode on the LCD which is Full Step Mode by default
  
  while(manualExit == 0){

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
  
  if(ps2x.ButtonPressed(PSB_L1)){ //if the L1 button is pressed then the resolution value is increased by a value of 1 (this moves from full step to half step etc)
      manualResolution = manualResolution + 1;
             
              if(manualResolution == 7){ //if the resolution value is 7 then the value wraps around to equal 2
                 manualResolution = 2;
              }
    
      Serial.println("L1 - Resolution Increase");
      Serial.println(manualResolution);
  
      lcd.clear(); //This section clears the LCD and Displays the new Step Mode
      
      if(manualResolution == 2){
        lcd.print("Full Step Mode");
      }
      
      else if(manualResolution == 3){
        lcd.print("Half Step Mode");
      }
      
      else if(manualResolution == 4){
        lcd.print("Quarter Step Mode");
      }
      
      else if(manualResolution == 5){
        lcd.print("Eighth Step Mode");
      }
      
      else if(manualResolution == 6){
        lcd.print("Sixteenth Step Mode");
      }
      
      else{
        lcd.print("Resolution Error");
      } 
  }
    
  if(ps2x.ButtonPressed(PSB_R1)){ //if the R1 button is pressed then the resolution value is decreased by a value of 1 (this moves from half step to full step etc)
      manualResolution = manualResolution - 1;
            
              if(manualResolution == 1){ //if the resolution value is 1 then the value wraps around to equal 6
                 manualResolution = 6;
              }
     
      Serial.println("R1 - Resolution Decrease");
      Serial.println(manualResolution);
    
      lcd.clear(); //This section clears the LCD and Displays the new Step Mode
    
      if(manualResolution == 2){
        lcd.print("Full Step Mode");
      }
      
      else if(manualResolution == 3){
        lcd.print("Half Step Mode");
      }
      
      else if(manualResolution == 4){
        lcd.print("Quarter Step Mode");
      }
      
      else if(manualResolution == 5){
        lcd.print("Eighth Step Mode");
      }
      
      else if(manualResolution == 6){
        lcd.print("Sixteenth Step Mode");
      }
      
      else{
        lcd.print("Resolution Error");
      } 
 }
    
    /*lcd.clear(); //the following section displays the X and Y values for the two analog sticks

    lcd.setCursor(3,0);
    lcd.print("X");
    lcd.setCursor(0,0);  
    lcd.print(ps2x.Analog(PSS_LX));

    lcd.setCursor(3,1);
    lcd.print("Y");
    lcd.setCursor(0,1);  
    lcd.print(ps2x.Analog(PSS_LY));

    lcd.setCursor(7,0);
    lcd.print("X");
    lcd.setCursor(4,0);  
    lcd.print(ps2x.Analog(PSS_RX));

    lcd.setCursor(7,1);
    lcd.print("Y");
    lcd.setCursor(4,1);  
    lcd.print(ps2x.Analog(PSS_RY));*/
    
      if(ps2x.ButtonPressed(PSB_GREEN)){ //if the Green Triangle button is pressed then we change the manualExit variable which will cause us to exit the Manual Control while statement and return to the start of the void loop function
        lcd.clear();
        lcd.print("Exiting to Menu");
        delay(1000);
        manualExit = 1;
      }
 
    } //end of manual mode while statement
   
}//end of void loop












/*

 Code to display welcome message on screen 
 */

#include <LiquidCrystal.h> // include the library to run the LCD

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // initialize the LCD library with the pin numbers of the LCD (RS, EN, D4, D5, D6, D7)


void setup(){
  lcd.begin(8,2); //sets the size of the LCD

  lcd.print("Welcome to CAMCON"); // welcome message
  delay(450); // initial delay before scrolling
  for (int positionCounter = 0; positionCounter < 17; positionCounter++) { //(Sets initial value, argument to exit, increases value with each loop)
    lcd.scrollDisplayLeft();
    delay(450); //sets delay between character scrolling
  }
}

void loop(){

  lcd.print ("Press X to Begin");



}





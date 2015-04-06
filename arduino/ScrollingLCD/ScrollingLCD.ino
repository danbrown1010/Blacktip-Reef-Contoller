#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//Addr: 0x3F, 20 chars & 4 lines
LiquidCrystal_I2C lcd(0x27,20,4); 

lcd.init(); 
// ------- Quick 3 blinks of backlight  -------------
  for(int i = 0; i< 3; i++)
  {
    lcd.backlight();
    delay(500);
    lcd.noBacklight();
    delay(500);
  }
  lcd.backlight(); // finish with backlight on  
 
//-------- Write characters on the display ------------------
  // NOTE: Cursor Position: Lines and Characters start at 0  
  lcd.setCursor(3,0); //Start at character 4 on line 0
  lcd.print("Hello, Reef!");
  delay(1000);
  lcd.setCursor(2,1);
  lcd.print("From BlackTip");
  delay(1000);  
  lcd.setCursor(0,2);
  lcd.print("Simple Display");
  lcd.setCursor(0,3);
  delay(2000);   
  lcd.print("www.blacktipreef.com");
  delay(5000);
  // Wait and then tell user they can start the Serial Monitor and type in characters to
  // Display. (Set Serial Monitor option to "No Line Ending")
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("Welcome to BlackTip");
  lcd.setCursor(0,1);
  lcd.print("Version 0.001");  


}/*--(end setup )---*/


void loop() {
  // scroll 13 positions (string length) to the left 
  // to move it offscreen left:
  for (int positionCounter = 0; positionCounter < 13; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft(); 
    // wait a bit:
    delay(150);
  }

  // scroll 29 positions (string length + display length) to the right
  // to move it offscreen right:
  for (int positionCounter = 0; positionCounter < 29; positionCounter++) {
    // scroll one position right:
    lcd.scrollDisplayRight(); 
    // wait a bit:
    delay(150);
  }
  
    // scroll 16 positions (display length + string length) to the left
    // to move it back to center:
  for (int positionCounter = 0; positionCounter < 16; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft(); 
    // wait a bit:
    delay(150);
  }
  
  // delay at the end of the full loop:
  delay(1000);

}










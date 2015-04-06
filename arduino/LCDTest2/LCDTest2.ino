#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define ONE_WIRE_BUS 7

OneWire ourWire(ONE_WIRE_BUS);

DallasTemperature sensors(&ourWire);
//Addr: 0x27, 20 chars & 4 lines
LiquidCrystal_I2C lcd(0x27,20,4); 
int incomingByte =0;
long currentmillis=0;

void setup()
{
  lcd.init(); 
  sensors.begin();
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature Test");
  
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


void loop()
{
 /*sensors.requestTemperatures();
 //Serial.print("Temperature for device 1 (index0) is: ");
 float x=sensors.getTempFByIndex(0);
 float y=sensors.getTempFByIndex(1);
 //Serial.println(sensors.getTempFByIndex(0));
 lcd.setCursor(0,2);
 lcd.print("T1: ");
 lcd.print(x);
 
 //lcd.setCursor(10,2);
 //lcd.print("T2: ");
 //lcd.print(y);
 
 
 // set the cursor to column 0, line 1
 // (note: line 1 is the second row, since counting begins with 0):
 lcd.setCursor(0, 3);
 lcd.print("UT=");
 currentmillis = millis(); 
  // print the number of seconds since reset:
  //lcd.print("Last rset = ");
 long days=0;
 long hours=0;
 long mins=0;
 long secs=0;
 secs = currentmillis/1000; //convect milliseconds to seconds
 mins=secs/60; //convert seconds to minutes
 hours=mins/60; //convert minutes to hours
 days=hours/24; //convert hours to days
 secs=secs-(mins*60); //subtract the coverted seconds to minutes in order to display 59 secs max 
 mins=mins-(hours*60); //subtract the coverted minutes to hours in order to display 59 minutes max
 hours=hours-(days*24); //subtract the coverted hours to days in order to display 23 hours max
 //Display results
   if (days>0) // days will displayed only if value is greater than zero
 {
   lcd.print(days);
   lcd.print(" days and :");
 }
 lcd.print(hours);
 lcd.print(":");
 lcd.print(mins);
 lcd.print(":");
 lcd.print(secs);   
  //lcd.print(uptime);
  //lcd.print("s");
  //SerialCheck (); //redirect ro serial check
  */
}





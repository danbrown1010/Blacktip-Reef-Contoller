#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/*-----( Declare Constants )-----*/
#define ONE_WIRE_BUS 7
#define RELAY_ON 0
#define RELAY_OFF 1
/*-----( Declare objects )-----*/
/*-----( Declare Variables )-----*/
#define Relay_1  2  // Arduino Digital I/O pin number
#define Relay_2  3
#define Relay_3  4
#define Relay_4  5
#define Relay_5  6
#define Relay_6  7
#define Relay_7  8
#define Relay_8  9

OneWire ourWire(ONE_WIRE_BUS);

DallasTemperature sensors(&ourWire);
//Addr: 0x27, 20 chars & 4 lines
LiquidCrystal_I2C lcd(0x27,20,4); 
int incomingByte =0;
long currentmillis=0;
float DT_threshold = 78.6; //Display Tank Temperature.
float QT_threshold = 78.6; //Quaratine Tank Holding Temp
float RODI_threshold = 78.6; // RODI Resevoir Holding Temp
float SALT_threshold = 78.6; // Salt Mix Resevoir Holding Temp
int onwait = 2000;

void setup() /****** SETUP: RUNS ONCE ******/
{
  //-------( Initialize Pins so relays are inactive at reset)----
  digitalWrite(Relay_1, RELAY_OFF);
  digitalWrite(Relay_2, RELAY_OFF);
  digitalWrite(Relay_3, RELAY_OFF);
  digitalWrite(Relay_4, RELAY_OFF); 
  digitalWrite(Relay_5, RELAY_OFF);
  digitalWrite(Relay_6, RELAY_OFF);
  digitalWrite(Relay_7, RELAY_OFF);
  digitalWrite(Relay_8, RELAY_OFF); 
  
  //---( THEN set pins as outputs )----  
  pinMode(Relay_1, OUTPUT);   
  pinMode(Relay_2, OUTPUT);  
  pinMode(Relay_3, OUTPUT);  
  pinMode(Relay_4, OUTPUT);    
  pinMode(Relay_5, OUTPUT);   
  pinMode(Relay_6, OUTPUT);  
  pinMode(Relay_7, OUTPUT);  
  pinMode(Relay_8, OUTPUT);    
  
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
  
  delay(2000);
  // Wait and then tell user they can start the Serial Monitor and type in characters to
  // Display. (Set Serial Monitor option to "No Line Ending")
  lcd.clear();
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("Welcome to BlackTip");
  lcd.setCursor(0,1);
  lcd.print("V 0.002"); 

}/*--(end setup )---*/


void loop()
{
   //---( Turn all 8 relays ON in sequence)---
  /*digitalWrite(Relay_1, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second
  digitalWrite(Relay_2, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second  
  digitalWrite(Relay_3, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second
  digitalWrite(Relay_4, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second  
  digitalWrite(Relay_5, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second
  digitalWrite(Relay_6, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second  
  digitalWrite(Relay_7, RELAY_ON);// set the Relay ON
  delay(onwait);              // wait for a second
  digitalWrite(Relay_8, RELAY_ON);// set the Relay ON
  delay(1000);              // wait see all relays ON
  
//---( Turn all 4 relays OFF in sequence)---  
  digitalWrite(Relay_1, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second
  digitalWrite(Relay_2, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second  
  digitalWrite(Relay_3, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second
  digitalWrite(Relay_4, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second
  digitalWrite(Relay_5, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second
  digitalWrite(Relay_6, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second  
  digitalWrite(Relay_7, RELAY_OFF);// set the Relay OFF
  delay(onwait);              // wait for a second
  digitalWrite(Relay_8, RELAY_OFF);// set the Relay OFF
  delay(1000);             */ // wait see all relays OFF 
 
 // 
 sensors.requestTemperatures();
 float dttemp=sensors.getTempFByIndex(0);
 float qttemp=sensors.getTempFByIndex(1);
 float rotemp=sensors.getTempFByIndex(2);
 float satemp=sensors.getTempFByIndex(3);
 
 lcd.setCursor(0,2);
 lcd.print("DT: ");
 lcd.print(dttemp);
 lcd.setCursor(11,2);
 lcd.print("QT: ");
 lcd.print(qttemp);
 lcd.setCursor(0,3);
 lcd.print("RO: ");
 lcd.print(rotemp);
 lcd.setCursor(11,3);
 lcd.print("SA: ");
 lcd.print(satemp);

//Display Tank Temp Control - 2 heaters
 if (dttemp > DT_threshold) {
   digitalWrite(Relay_8, RELAY_OFF);
   digitalWrite(Relay_7, RELAY_OFF);
 }else {
   digitalWrite(Relay_8, RELAY_ON);
   digitalWrite(Relay_7, RELAY_ON);
 } 
 
//QT Tank Temp Control - 1 heater
  if (qttemp > QT_threshold) {
   digitalWrite(Relay_1, RELAY_OFF);
 }else {
   digitalWrite(Relay_1, RELAY_ON);
 } 
 
 //RO Tank Temp Control - 1 heater
  if (rotemp > QT_threshold) {
   digitalWrite(Relay_2, RELAY_OFF);
 }else {
   digitalWrite(Relay_2, RELAY_ON);
 } 
 
 //SA Tank Temp Control - 1 heater
  if (satemp > QT_threshold) {
   digitalWrite(Relay_3, RELAY_OFF);
 }else {
   digitalWrite(Relay_3, RELAY_ON);
 } 
 
 //lcd.setCursor(10,2);
 //lcd.print("T2: ");
 //lcd.print(y);
 
 
 // set the cursor to column 8, line 2
 lcd.setCursor(8, 1);
 lcd.print("UT=");
 currentmillis = millis(); 
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
  
}





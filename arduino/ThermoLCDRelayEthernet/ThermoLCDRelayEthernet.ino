#include <SimpleTimer.h>
#include <dht11.h>
#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

/*-----( Declare Constants )-----*/

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

#define DHT11PIN A0
#define ONE_WIRE_BUS A1
dht11 DHT11;

// the timer objects
SimpleTimer temperatureTimer;
SimpleTimer cloudUpdateTempTimer;
SimpleTimer cloudUpdateHumTimer;

OneWire ourWire(ONE_WIRE_BUS);

DallasTemperature sensors(&ourWire);
//Addr: 0x27, 20 chars & 4 lines
LiquidCrystal_I2C lcd(0x27,20,4); 
int incomingByte=0;
long currentmillis=0;

//Setup Temperatures
float DT_threshold = 78.6; //Display Tank Temperature.
float QT_threshold = 78.6; //Quaratine Tank Holding Temp
float RODI_threshold = 78.6; // RODI Resevoir Holding Temp
float SALT_threshold = 78.6; // Salt Mix Resevoir Holding Temp
float dttemp=78.6;
float qttemp=78.6;
float rotemp=78.6;
float satemp=78.6;
float roomTemp=78.6;
float roomHum=0.0;

int onwait = 500;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "www.google.com";
char btrServer[] = "www.blacktipreef.com";
String account_sid = "AC29fd98db5431536e202df77d77769575";
String auth_token = "00a3b335a977d709380bdeaa53780da4";
char vers[] = "v0.2";
IPAddress ip(192, 168, 0, 177);
EthernetClient client;
//Timers
int TEMP_READ_INTERVAL = 1000;          // Default to 1 sec
int TEMP_CLOUD_UPDATE_INTERVAL = 60000; // Default to 1 min 
int HUM_CLOUD_UPDATE_INTERVAL = 600000; // Default to 10 mins


//Texting Conditions
String REBOOT = "0";
String HIGHTEMP = "1";
String LOWTEMP = "2";




void setup() /****** SETUP: RUNS ONCE ******/
{
  // start serial port
  Serial.begin(9600);
  temperatureTimer.setInterval(TEMP_READ_INTERVAL, readAndDisplayTemperatures);    // Every 1 sec
  cloudUpdateTempTimer.setInterval(TEMP_CLOUD_UPDATE_INTERVAL, sendTemperaturesToCloud); // Every 1 mins
  cloudUpdateHumTimer.setInterval(HUM_CLOUD_UPDATE_INTERVAL, sendHumidityToCloud);      // Every 10 mins
  initializeLCD();
  initializeNetwork();
  initializeRelay();
  sensors.begin();
  //---LOG THE REBOOT-----------------------------------------
  sendTxtMessage(REBOOT);//Reboot Message that is converted on server.
  sendMeasurement("QT","REBOOT","LOG");
  sendMeasurement("DT","REBOOT","LOG");
  sendMeasurement("RO","REBOOT","LOG");
  sendMeasurement("SA","REBOOT","LOG");
  sendMeasurement("Rm","REBOOT","LOG");
  //-----------------------------------------------------------
  // If everything worked to this point --> Welcome Screen
  //-----------------------------------------------------------
  lcd.clear();
  //lcd.setCursor(0,0); //Start at character 0 on line 0
  //lcd.print("Welcome to BlackTip");
  lcd.setCursor(0,0);
  lcd.print(vers); 

}/*--(end setup )---*/


void loop()
{
   // if there are incoming bytes available
  // from the server, read them and print them:
 temperatureTimer.run();
 cloudUpdateTempTimer.run(); 
 cloudUpdateHumTimer.run(); 
 // set the cursor to column 8, line 2
 lcd.setCursor(8, 0);
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
 
 
 

  
}

//-------Initialize the Network-----------------------------------
void initializeNetwork(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Init network...");
  delay(3000);
  if (Ethernet.begin(mac) == 0) {
    lcd.setCursor(0,1);
    lcd.println("DHCP Failure :( ");
    delay(3000);
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connecting...");
  delay(2000);
}

//-------Send Txt Message-----------------------------------www.twilio.com/2010-04-01/Accounts/AC29fd98db5431536e202df77d77769575/Messages
void sendTxtMessage(String message){
  if (client.connect(server, 80)) { 
    Serial.println("connected...sending text...");
    Serial.print("message=");
    Serial.println(message);
    // Make a HTTP request:
    client.print("GET /sendtext?body=");
    client.print(message);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(btrServer);
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }

}


//-------Send Temperatures to Cloud-----------------------------------
void sendTemperaturesToCloud(){
   lcd.clear();
   lcd.setCursor(0,1);
   lcd.print("Sending Temperatures to Blacktip...");
   sendFloatMeasurement("QT",qttemp,"TEMP");
   sendFloatMeasurement("DT",dttemp,"TEMP");
   sendFloatMeasurement("RO",rotemp,"TEMP");
   sendFloatMeasurement("SA",satemp,"TEMP");
   sendFloatMeasurement("Rm",roomTemp,"TEMP");
   if (dttemp>DT_threshold){
     sendTxtMessage(HIGHTEMP);
   }
   lcd.clear();
}

void sendHumidityToCloud(){
   lcd.clear();
   lcd.setCursor(0,1);
   lcd.print("Sending Humidity to Blacktip...");
   sendFloatMeasurement("Rm",roomHum,"HUMIDITY");
   lcd.clear();
}



void readAndDisplayTemperatures(){
 int chk = DHT11.read(DHT11PIN);

 Serial.print("Temp\Hum Sensor: ");
 switch (chk)
 {
    case DHTLIB_OK: 
		Serial.println("OK"); 
		break;
    case DHTLIB_ERROR_CHECKSUM: 
		Serial.println("Checksum error"); 
		break;
    case DHTLIB_ERROR_TIMEOUT: 
		Serial.println("Time out error"); 
		break;
    default: 
		Serial.println("Unknown error"); 
		break;
 }

 roomHum = (float)DHT11.humidity;
 roomTemp = (float)Fahrenheit(DHT11.temperature); 
 sensors.requestTemperatures();
 dttemp=sensors.getTempFByIndex(0);
 qttemp=sensors.getTempFByIndex(1);
 rotemp=sensors.getTempFByIndex(2);
 satemp=sensors.getTempFByIndex(3);
 
 lcd.setCursor(0,0);
 lcd.print(vers);
 lcd.setCursor(0,1);
 lcd.print("Rm:");
 lcd.print(roomTemp);
 lcd.setCursor(11,1);
 lcd.print("H%:");
 lcd.print(roomHum);
 lcd.setCursor(0,2);
 lcd.print("DT:");
 lcd.print(dttemp);
 lcd.setCursor(11,2);
 lcd.print("QT:");
 lcd.print(qttemp);
 lcd.setCursor(0,3);
 lcd.print("RO:");
 lcd.print(rotemp);
 lcd.setCursor(11,3);
 lcd.print("SA:");
 lcd.print(satemp);
 

 
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
 
}

void initializeLCD(){
// ------- Quick 3 blinks of backlight  ------------- 
  lcd.init(); 
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
  lcd.print(" Simple Display");
  lcd.setCursor(0,3);
  delay(1000);   
  lcd.print("www.blacktipreef.com");
  delay(1000);
}


void initializeRelay(){
    //-------( Initialize Pins so relays are inactive at reset)----
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Init Relays...");  
  
  
  //---( THEN set pins as outputs )----  
  pinMode(Relay_1, OUTPUT);   
  pinMode(Relay_2, OUTPUT);  
  pinMode(Relay_3, OUTPUT);  
  pinMode(Relay_4, OUTPUT);    
  pinMode(Relay_5, OUTPUT);   
  pinMode(Relay_6, OUTPUT);  
  pinMode(Relay_7, OUTPUT);  
  pinMode(Relay_8, OUTPUT);    


  lcd.setCursor(0,3);
  lcd.print("Testing 1");
  digitalWrite(Relay_1, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_1, RELAY_OFF);
  
  lcd.setCursor(0,3);
  lcd.print("Testing 2");
  digitalWrite(Relay_2, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_2, RELAY_OFF);

  lcd.setCursor(0,3);
  lcd.print("Testing 3");  
  digitalWrite(Relay_3, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_3, RELAY_OFF);
  
  lcd.setCursor(0,3);
  lcd.print("Testing 4");  
  digitalWrite(Relay_4, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_4, RELAY_OFF); 
  
  lcd.setCursor(0,3);
  lcd.print("Testing 5");  
  digitalWrite(Relay_5, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_5, RELAY_OFF);
  
  lcd.setCursor(0,3);
  lcd.print("Testing 6");  
  digitalWrite(Relay_6, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_6, RELAY_OFF);
  
  lcd.setCursor(0,3);
  lcd.print("Testing 7");  
  digitalWrite(Relay_7, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_7, RELAY_OFF);
  
  lcd.setCursor(0,3);
  lcd.print("Testing 8");  
  digitalWrite(Relay_8, RELAY_ON);
  delay(onwait); 
  digitalWrite(Relay_8, RELAY_OFF); 

}



void sendFloatMeasurement(char* tankId, float value, char* type){
  static char outstr[15];
  dtostrf(value,5, 2, outstr);
  sendMeasurement(tankId,outstr,type);
}

void sendMeasurement(char* tankId, char* value, char* type){
  if (client.connect(server, 80)) { 
    Serial.println("connected...sending measurement...");
    Serial.print(tankId);
    Serial.print("=");
    Serial.println(value);
    Serial.print("=");
    Serial.println(type);
    // Make a HTTP request:
    client.print("GET /tankmeasurement?tankId=");
    client.print(tankId);
    client.print("&measurementContent=");
    client.print(value);
    client.print("&measurementType=");
    client.print(type);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(btrServer);
    client.println("Connection: close");
    client.println();
    client.stop();
  }
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
  }
}

//Celsius to Fahrenheit conversion
double Fahrenheit(double celsius)
{
	return 1.8 * celsius + 32;
}


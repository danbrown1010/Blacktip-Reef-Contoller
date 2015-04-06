#include <SimpleTimer.h>
#include <dht11.h>
#include <Ethernet.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

/*-----( Declare Constants )-----*/
#define RELAY_ON 0
#define RELAY_OFF 1
#define Relay_1  A8  
#define Relay_2  A9
#define Relay_3  A10
#define Relay_4  A11
#define Relay_5  A12
#define Relay_6  A13
#define Relay_7  A14
#define Relay_8  A15
#define DHT11PIN A0
#define ONE_WIRE_BUS A1
const float DT_threshold=78.6; //Display Tank Temperature.
const float QT_threshold=78.6; //Quaratine Tank Holding Temp
const float RODI_threshold=78.6; // RODI Resevoir Holding Temp
const float SALT_threshold=78.6; // Salt Mix Resevoir Holding Temp
const long onwait = 500;
const char server[] = "www.google.com";
//const char btrServer[] = "www.blacktipreef.com";
const char btrServer[] = "sixth-tempo-89216.appspot.com";
const int chipSelect = 4;
const String account_sid = "AC29fd98db5431536e202df77d77769575";
const String auth_token = "00a3b335a977d709380bdeaa53780da4";
const char vers[] = "v0.2";

//Timer Intervals
const long TEMP_READ_INTERVAL = 1000;          // Default to 1 sec
const long TEMP_CLOUD_UPDATE_INTERVAL = 60000; // Default to 1 min 
const long HUM_CLOUD_UPDATE_INTERVAL = 600000; // Default to 10 mins

//Texting Conditions
String REBOOT = "0";
String HIGHTEMP = "1";
String LOWTEMP = "2";

/*-----( Instantiate objects )-----*/
SimpleTimer temperatureTimer;
SimpleTimer cloudUpdateTempTimer;
SimpleTimer cloudUpdateHumTimer;
dht11 DHT11;
Sd2Card card;
SdVolume volume;
SdFile root;
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);
LiquidCrystal_I2C lcd(0x27,20,4);
IPAddress ip(192, 168, 0, 177);
EthernetClient client;

/*-----( Declare Variables )-----*/
long currentmillis=0;
float dttemp=78.6;
float qttemp=78.6;
float rotemp=78.6;
float satemp=78.6;
float roomTemp=78.6;
float roomHum=0.0;
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };


//==============================================
//-----( Setup Runs Once )----------------------
//==============================================
void setup()
{
  // start serial port
  Serial.begin(9600);
  temperatureTimer.setInterval(TEMP_READ_INTERVAL, readAndDisplayTemperatures);    // Every 1 sec
  cloudUpdateTempTimer.setInterval(TEMP_CLOUD_UPDATE_INTERVAL, sendTemperaturesToCloud); // Every 1 mins
  cloudUpdateHumTimer.setInterval(HUM_CLOUD_UPDATE_INTERVAL, sendHumidityToCloud);      // Every 10 mins
  initializeLCD();
  initializeNetwork();
  initializeRelay();
  initializeSDCard();
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

//==============================================
//-----( Main Loop )----------------------------
//==============================================
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

//==============================================
//-----( Initialize the Network )---------------
//==============================================
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


//==============================================
//-----( Initialize the SD Card )---------------
//==============================================
void initializeSDCard(){
  

  Serial.print("\nInitializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);     // change this to 53 on a mega


  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card is inserted?");
    Serial.println("* Is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
}


//==============================================
//-----( Send Text to Servlet )-----------------
//==============================================
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


//==============================================
//-----( Send Temps to Cloud )------------------
//==============================================
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

//==============================================
//-----( Send Humidy to Cloud )-----------------
//==============================================
void sendHumidityToCloud(){
   lcd.clear();
   lcd.setCursor(0,1);
   lcd.print("Sending Humidity to Blacktip...");
   sendFloatMeasurement("Rm",roomHum,"HUMIDITY");
   lcd.clear();
}


//==============================================
//---( Read and Display Temps to LCD )----------
//==============================================
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

//==============================================
//---( Initialize the LCD )---------------------
//==============================================
void initializeLCD(){
// ------- Quick 2 blinks of backlight  ------------- 
  lcd.init(); 
  for(int i = 0; i< 2; i++)
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

//==============================================
//---( Initialize the 8 Relays )----------------
//==============================================
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


//==============================================
//---( Send a measurment of type Float )--------
//==============================================
void sendFloatMeasurement(char* tankId, float value, char* type){
  static char outstr[15];
  dtostrf(value,5, 2, outstr);
  sendMeasurement(tankId,outstr,type);
}

//==============================================
//---( Send a measurment of type char* )--------
//==============================================
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


//==============================================
//---( Convert Celcius to Fahrenheit )----------
//==============================================
double Fahrenheit(double celsius)
{
	return 1.8 * celsius + 32;
}


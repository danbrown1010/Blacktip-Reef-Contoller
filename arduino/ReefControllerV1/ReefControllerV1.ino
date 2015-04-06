/*
  ReefController

 This sketch connects to a configuration website and downloads
 configuration details. Then saves those conifgs to a file for 
 dynamic configuration using an Arduino Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 26 Mar 2015
 by Dan Brown

 */

#include <SPI.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 

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
const long onwait = 100;
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);
OneWire ourWire(ONE_WIRE_BUS);
DallasTemperature sensors(&ourWire);
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;
LiquidCrystal_I2C lcd(0x27,20,4);


File myFile;
char* filename = "btstrp01.txt";
String response;
String configStr;
boolean useOldConfig = false;
char c;
char buf[8];

char start_token = '$';
char end_token = '*';


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);
  
  initializeLCD();
  logMajorEvent("BootStrap","-S1: SD Card Init---");
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  logMajorEvent("BootStrap","-S2: Network Init---");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("Connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected!");
    logMajorEvent("BootStrap","-S3: Get Web Cfgs---");
    getConfigStringFromWeb();
  }
  else {
    // You didn't get a connection to the server:
    Serial.println("Connection failed...using config from last good boot!");
    useOldConfig = true;
  }
  
  if (!useOldConfig){
     logMajorEvent("BootStrap","-S4: Save Cfgs (SD)-");
     writeConfigsToSD();  
  } else {
    Serial.println("USING OLD CONFIG FILE");
  }
  
  if (!client.connected()) {
    //Serial.println();
    logMajorEvent("BootStrap","-S5: Close Connxn---");
    client.stop();
  }
  
  logMajorEvent("BootStrap","-S6: Init Relays----");
  initializeRelay();
  sensors.begin();
  //---LOG THE REBOOT-----------------------------------------
  //sendTxtMessage(REBOOT);//Reboot Message that is converted on server.
  //sendMeasurement("QT","REBOOT","LOG");
  //sendMeasurement("DT","REBOOT","LOG");
  //sendMeasurement("RO","REBOOT","LOG");
  //sendMeasurement("SA","REBOOT","LOG");
  //sendMeasurement("Rm","REBOOT","LOG");
  //-----------------------------------------------------------
  // If everything worked to this point --> Welcome Screen
  //-----------------------------------------------------------
  lcd.clear();
  //lcd.setCursor(0,0); //Start at character 0 on line 0
  //lcd.print("Welcome to BlackTip");
  lcd.setCursor(0,0);
  lcd.print(" Boot Load Complete "); 
  lcd.setCursor(0,1); 
  lcd.print(" Welcome to BlackTip");
  lcd.setCursor(0,2); 
  lcd.print("       v0.2         ");
  delay(3000);
}

void loop()
{
  // Nothing yet
}

//==============================================
//---( Write New configs to SD )----------------
//==============================================
void writeConfigsToSD(){     
     if (SD.exists(filename)) {
        Serial.print(filename);
        Serial.println(" exists.");
        SD.remove(filename);
     }
     else {
        Serial.print(filename);
        Serial.println(" does NOT exist.");
     }
     delay(300);
     myFile = SD.open(filename, FILE_WRITE);     
     Serial.print("Writing to ");
     Serial.println(filename);
     if (myFile) {
        myFile.print(configStr);
        myFile.close();
        Serial.println("done.");
     } else {
        Serial.print("Error opening: ");
        Serial.println(filename);
     }

     myFile = SD.open(filename);
     Serial.print("Reading from ");
     Serial.println(filename);
     if (myFile) {
         // read from the file until there's nothing else in it:
        while (myFile.available()) {
           Serial.write(myFile.read());
         }
       Serial.write("\n");
       myFile.close();
      } else {
        Serial.print("Error opening: ");
        Serial.println(filename);
      }
}


//==============================================
//---( Get Configs from Web )---------------------
//==============================================
void getConfigStringFromWeb(){ 
    // Make a HTTP request:
    client.println("GET /readconfig HTTP/1.1");
    client.println("Host: sixth-tempo-89216.appspot.com");
    //client.println("Connection: close");
    client.println();
    delay(500);
    while(client.connected() && !client.available()) delay(1); 
    while (client.available()) {
        c = client.read();
        response= response + c;
      }
     Serial.println("HTTP 1.1 Response Recieved!");
     int contentBodyIndex = response.lastIndexOf('\r\n\r\n'); //Should be between header and body
     if (contentBodyIndex > 0) {
        configStr = response.substring(contentBodyIndex+1); //add one to the index to strip off the leading lf
        Serial.println("Stripped HTTP Header.");
     }
     client.stop();
     client.flush();
     Serial.print("Final config string:");
     delay(100);
     Serial.println(configStr);
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
    delay(300);
    lcd.noBacklight();
    delay(300);
  }
  lcd.backlight(); // finish with backlight on  
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
//---( Log events to LCD and Serial )-----------
//==============================================

void logMajorEvent(String phase, String message){
  Serial.println(message);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Event: "+phase);
  lcd.setCursor(0,2);
  lcd.print(message);
}

void setAllVariables(){
}

//==============================================
//---( Initialize the 8 Relays )----------------
//==============================================
void initializeRelay(){
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





void setUpGlobalVars(){
     myFile = SD.open(filename);
     Serial.print("Reading from ");
     Serial.println(filename);
     if (myFile) {
         // read from the file until there's nothing else in it:
        while (myFile.available()) {
           String fileStr = myFile.read();
           int endTokenIndex = filestr.lastIndexOf(end_token); //Should be between header and body
           int startTokenIndex = filestr.lastIndexOf(start_token); //Should be between header and body
           // save substring
           // and shorten the string- but how?
           if (contentBodyIndex > 0) {
               configStr = response.substring(contentBodyIndex+1); //add one to the index to strip off the leading lf
           Serial.println("Stripped HTTP Header.");
           }
          }
        myFile.close();
     } else {
        Serial.print("Error opening: ");
        Serial.println(filename);
     }
 }
      
      
  

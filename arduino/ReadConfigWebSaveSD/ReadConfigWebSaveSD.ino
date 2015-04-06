/*--------------------------------------------------------------
  Program:      eth_websrv_SD

  Description:  Arduino web server that serves up a basic web
                page. The web page is stored on the SD card.
  
  Hardware:     Arduino Uno and official Arduino Ethernet
                shield. Should work with other Arduinos and
                compatible Ethernet shields.
                2Gb micro SD card formatted FAT16
                
  Software:     Developed using Arduino 1.0.3 software
                Should be compatible with Arduino 1.0 +
                SD card contains web page called index.htm
  
  References:   - WebServer example by David A. Mellis and 
                  modified by Tom Igoe
                - SD card examples by David A. Mellis and
                  Tom Igoe
                - Ethernet library documentation:
                  http://arduino.cc/en/Reference/Ethernet
                - SD Card library documentation:
                  http://arduino.cc/en/Reference/SD

  Date:         10 January 2013
 
  Author:       W.A. Smith, http://startingelectronics.com
--------------------------------------------------------------*/

#include <Ethernet.h>
#include <OneWire.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>



// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
Sd2Card card;
SdVolume volume;
SdFile root;
LiquidCrystal_I2C lcd(0x27,20,4);
IPAddress ip(192, 168, 0, 177);
EthernetClient client;
const char server[] = "www.google.com";
File webFile;
const char btrServer[] = "sixth-tempo-89216.appspot.com";
const String device_id="10101010";
const int chipSelect = 4;

void setup()
{
    delay(5000);
    Serial.begin(9600);
    initializeLCD();
    initializeNetwork();
    initializeSD();
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    updateConfigsToSD();
}

void loop()
{
}



//==============================================
//---( Read configs from Site and save to SD)---
//==============================================
void updateConfigsToSD(){
if (client.connect(server, 80)) { 
    // Make a HTTP request:
    client.print("GET /readconfig?deviceId=");
    client.print(device_id);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(btrServer);
    client.println("Connection: close");
    client.println();
  }
  else {
    // kf you didn't get a connection to the server:
    displayEvent("connection failed");
  }

  lcd.setCursor(0,0);
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    lcd.println(c);
  }

  client.stop();

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

}



//==============================================
//---( Log major events to LCD and Serial )-----
//==============================================
void displayEvent(String text){
  Serial.println(text);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(text);
  delay(1500);
}

//==============================================
//---( Initialize the SDCard )---------------------
//==============================================
void initializeSD(){
    pinMode(53, OUTPUT);     // 53 on a mega
    
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        displayEvent("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    displayEvent("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        displayEvent("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
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
//-----( Initialize the Network )---------------
//==============================================
void initializeNetwork(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Init network...");
  delay(3000);
  if (Ethernet.begin(mac) == 0) {
    lcd.setCursor(0,1);
    displayEvent("DHCP Failure :( ");
    delay(3000);
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  lcd.clear();
  lcd.setCursor(0,0);
  displayEvent("Connecting...");
  delay(2000);
}

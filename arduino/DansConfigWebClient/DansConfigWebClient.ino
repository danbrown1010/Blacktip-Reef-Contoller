/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen

 */

#include <SPI.h>
#include <Ethernet.h>
#include <SPI.h>
#include <SD.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

File myFile;
char* filename = "btstrp01.txt";
String response;
String configStr;
boolean useOldConfig = false;
char c;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.print("connecting...");

  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected!");
    getConfigStringFromWeb();
  }
  else {
    // You didn't get a connection to the server:
    Serial.println("Connection failed...using config from last good boot!");
    useOldConfig = true;
  }
  
  if (!useOldConfig){
     writeConfigsToSD();  
  } else {
    Serial.println("USING OLD CONFIG FILE");
  }
  
  
  
}

void loop()
{
  //Serial.println("Entering Loop");
  

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    //Serial.println();
    Serial.println("disconnecting.");
    client.stop();

    // do nothing forevermore:
    while (true);
  }
  
}


void writeConfigsToSD(){
     
     if (SD.exists(filename)) {
        Serial.println("example.txt exists.");
        SD.remove(filename);
     }
     else {
        Serial.println("example.txt doesn't exist.");
     }
     //Create empty file
     //myFile = SD.open(filename, FILE_WRITE);
     //myFile.close;
     delay(300);
     //Open for writing
     myFile = SD.open(filename, FILE_WRITE);     
     // if the file opened okay, write to it:
     Serial.print("Writing to ");
     Serial.println(filename);
     if (myFile) {
        //Serial.print("Writing to testXXX.txt...");
        myFile.print(configStr);
        // close the file:
        myFile.close();
        Serial.println("done.");
     } else {
    // if the file didn't open, print an error:
        Serial.print("Error opening: ");
        Serial.println(filename);
     }

     // re-open the file for reading:
     myFile = SD.open(filename);
     Serial.print("Reading from ");
     Serial.println(filename);
     if (myFile) {
         // read from the file until there's nothing else in it:
        while (myFile.available()) {
           Serial.write(myFile.read());
         }
       // close the file:
       myFile.close();
      } else {
      // if the file didn't open, print an error:
        Serial.print("Error opening: ");
        Serial.println(filename);
      }
}








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
      Serial.print(response);
}


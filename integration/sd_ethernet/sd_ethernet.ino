#include <SPI.h>
#include <Ethernet2.h>
#include <SD.h>

byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xC6, 0x97 }; //physical mac address
byte ip[] = { 192, 168, 0, 2 }; // local area network ip
byte gateway[] = { 192, 168, 1, 254 }; // found via route get default in terminal
byte subnet[] = { 255, 255, 255, 0 }; // standard subnet mask
EthernetServer server(80); //server port

//CS pin for Arduino Uno
//const int chipSelect = 8;
const int chipSelect = 4; //for Ethernet shield
const int imageWidthBytes = 8;

String readString; 

//////////////////////

void setup(){

  pinMode(13, OUTPUT); // pin selected to control. Built-in LED for demo.
  digitalWrite(13, LOW); // set it to low to it starts off
  //start Ethernet
  Ethernet.begin(mac, ip, gateway, subnet); // start the Ethernet
  server.begin(); // start the server

  //enable serial data print 
  Serial.begin(9600); 
  Serial.println("Initializing SD card...");
  
  //set pin 10 on Arduino Uno to OUTPUT otherwise SD libary functions won't work
  pinMode(4, OUTPUT);

  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");

  convertToByte();
  
}

void write(){
  File testFile = SD.open("testfile.txt", FILE_WRITE);
  if(testFile) {
    testFile.write("File created");
    testFile.close();
    Serial.println("File created");
  }
  else{
    Serial.println("Error on opening the file");
  }
}

void read(){
  File dataFile = SD.open("b_binary.txt");
  if(!dataFile) {
    Serial.println("Error on opening the file");
    return;
  }
  while(dataFile.available()){
    Serial.write(dataFile.read());
  }
  dataFile.close();
}

void convertToByte(){
  File dataFile = SD.open("b_binary.txt");
  if(!dataFile) {
    Serial.println("Error on opening the file");
    return;
  }
  int bitCounter = 8;
  int regCounter = 0;
  byte shiftReg[imageWidthBytes] = {0};
  byte temp;
  while(dataFile.available()){
    temp = dataFile.read();
    if(temp == '0') {
      bitCounter--;
      shiftReg[regCounter] = shiftReg[regCounter] << 1;
    }
    else if(temp == '1') {
      bitCounter--;
      shiftReg[regCounter] = (shiftReg[regCounter] << 1) + 1;
    }
    if(bitCounter == 0) {
      Serial.print(regCounter);
      Serial.print(':');
      Serial.print(shiftReg[regCounter], BIN);
      Serial.print('\t');
      bitCounter = 8;
      regCounter++;
    }
    if(regCounter == 8){
      Serial.print('\n');
      regCounter = 0;
    }
  }
  dataFile.close();
}

void loop(){
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100) {

          //store characters to string 
          readString += c; 
          //Serial.print(c);
        } 

        //if HTTP request has ended
        if (c == '\n') {

          ///////////////
          Serial.println(readString); //print to serial monitor for debuging 

          //now output HTML data header
          if(readString.indexOf('?') >=0) {
           client.println("HTTP/1.1 204 Null");
           client.println();
           client.println();  
          }
          else {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
  
            client.println("<HTML>");
            client.println("<HEAD>");
            client.println("</HEAD>");
            client.println("<BODY>");
  
            client.println("<H1>Digital Waterfall Test</H1>");
            
            client.println("<a href=\"/?write\" target=\"inlineframe\">WRITE</a>");
            client.println("<a href=\"/?read\" target=\"inlineframe\">READ</a>");
  
            client.println("<IFRAME name=inlineframe style=\"display:none\" >");          
            client.println("</IFRAME>");
  
            client.println("</BODY>");
            client.println("</HTML>");
          }

          delay(1);
          //stopping client
          client.stop();

          ///////////////////// control arduino pin
          if(readString.indexOf("write") >0) {
            write();
            Serial.println("File written.");
            break;
          }
          else if(readString.indexOf("read") >0) {
            read();
            Serial.println("File read.");
            break;
          }
          //clearing string for next read
          readString="";

        }
      }
    }
  }
} 

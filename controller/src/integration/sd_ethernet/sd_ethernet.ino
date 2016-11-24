#include <SPI.h>
#include <Ethernet2.h>
#include <SD.h>

// START OF CODE FOR CONTROLLING THE SHIFT REGISTERS AND LEDS  //
//////////////////////////////////////////////////////////////////////////////
//#include "TimerOne.h"
#include <MsTimer2.h>

#define SRCLR_PIN 9  //Active low
#define SRCLK_PIN 8
#define RCLK_PIN 7
#define OE_PIN 6     //Active Low

#define VALVES_PER_VALVE_BANK 8
#define NUM_VALVE_BANKS 2
#define NUM_VALVES NUM_VALVE_BANKS * VALVES_PER_VALVE_BANK

//TODO: Likely change this to come from the pattern file
#define PATTERN_LENGTH 16

#define SER1_PIN 2
#define SER2_PIN 3

int serial_pins[NUM_VALVE_BANKS] = {SER1_PIN, SER2_PIN};

byte pattern[PATTERN_LENGTH][NUM_VALVES];

int current_pattern_line = 0;

void update_output(void)
{
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW); 
  
  current_pattern_line = (current_pattern_line + 1) % PATTERN_LENGTH;
  
  //Iterate through the 8 bits being sent to a shift register for a given row (prepare the next row for output)
  int j;
  for (j = 0; j < VALVES_PER_VALVE_BANK; j++)
  {
    //Iterate through the shift registers, sending one bit to each
    int k;
    for (k = 0; k < NUM_VALVE_BANKS; k++)
    {
      if (pattern[current_pattern_line][j + (VALVES_PER_VALVE_BANK * k)])
        digitalWrite(serial_pins[k], HIGH);
      else
        digitalWrite(serial_pins[k], LOW);
    }
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }
}

void setup_shiftregs() {
  //Timer1.initialize(1000000);         // initialize timer1, and set a 1 second period
  //Timer1.attachInterrupt(update_output);  // attaches update_output() as a timer overflow interrupt
  MsTimer2::set(1000, (&update_output));

  digitalWrite(SRCLR_PIN, HIGH);
  digitalWrite(OE_PIN, LOW);
}
// END OF CODE FOR CONTROLLING THE SHIFT REGISTERS AND LEDS  //
//////////////////////////////////////////////////////////////////////////////

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

  pinMode(SRCLR_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(SER1_PIN, OUTPUT);
  pinMode(SER2_PIN, OUTPUT);

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
            
            setup_shiftregs();
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

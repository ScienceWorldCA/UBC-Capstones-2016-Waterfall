#include <SPI.h>
#include <SD.h>
#include "valve_output.h"
#include <MsTimer2.h>

//byte shiftReg[16][2] = {0};
byte *shiftReg;

//CS pin for Arduino Uno
const int chipSelect = 8;
//const int chipSelect = 4; //for Ethernet shield
//const int imageWidthBytes = 8;
const int imageWidthBytes = 1;
const int lineWidth = 4;

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

  //enable serial data print 
  Serial.begin(9600); 
  Serial.println("Initializing SD card...");

  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");

  //convertToByte();
  
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
  File dataFile = SD.open("middleon.txt");
  if(!dataFile) {
    Serial.println("Error on opening the file");
    return;
  }
  while(dataFile.available()){
    Serial.write(dataFile.read());
  }
  dataFile.close();
}

void checkCard(){
  boolean flag = SD.exists("middleon.txt");
  if(flag){
    Serial.println("File exists");
  }else{
    Serial.println("File does not exist");
  }
}

void convertToByte(){
  File dataFile = SD.open("middleon.txt");
  if(!dataFile){
    Serial.println("Error on opening the file");
    return;
  }
  //int lines = dataFile.size() / lineWidth;
  int lines = 6;
  int total_size = lines * imageWidthBytes;
  shiftReg = (byte*)malloc(total_size * sizeof(byte));
  memset(shiftReg, 0, sizeof(byte) * total_size);
  int bitCounter = 3;
  int regCounter = 0;
  int lineCounter = 0;
  //int lineIndex = (lines - 1) * imageWidthBytes;
  int startIndex = lines - 1;
  byte temp;
  while(1){
    temp = dataFile.read();
    if(temp == '0'){
      bitCounter--;
      shiftReg[startIndex] = shiftReg[startIndex] << 1;
    }
    else if(temp == '1'){
      bitCounter--;
      shiftReg[startIndex] = (shiftReg[startIndex] << 1) + 1;
    }
    /*
    if(bitCounter == 0){
      bitCounter = 3;
      regCounter++;
    }*/
    //change number of regCounter when width is different
    if(bitCounter == 0){
      bitCounter = 3;
      //regCounter = 0;
      lineCounter++;
      startIndex--;
    }
    if(startIndex < 0){
      break;
    }
  }
  dataFile.close();
  int i, j;
  for(i = 0; i < lines; i++){
    //for(j = 0; j < imageWidthBytes; j++){
      Serial.print(i);
      Serial.print(':');
      //Serial.print(shiftReg[i][j], BIN);
      //int index = i * imageWidthBytes + j;
      Serial.print(shiftReg[i], BIN);
      Serial.print('\t');
   // }
    Serial.print('\n');
  }
}

void loop(){
  convertToByte();
  setup_shiftregs();
  while(1)
  {
    if (pattern_load_high_low)
      load_shiftreg_high_low(shiftReg);
    if (pattern_load_low_high)
      load_shiftreg_low_high(shiftReg);
  }
} 

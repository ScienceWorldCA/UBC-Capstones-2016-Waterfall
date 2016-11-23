#include <SPI.h>
#include <SD.h>

//CS pin for Arduino Uno
const int chipSelect = 8;
//const int chipSelect = 4; //for Ethernet shield
const int imageWidthBytes = 8;

void setup() {
  // put your setup code here, to run once:

  //opens serial port, sets data rate to 9600bps
  Serial.begin(9600);
  Serial.println("Initializing SD card...");
  
  //set pin 10 on Arduino Uno to OUTPUT otherwise SD libary functions won't work
  pinMode(10, OUTPUT);

  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");

  convertToByte();
  //write();
}

void write(){
  File testFile = SD.open("testfile.txt", FILE_WRITE);
  if(testFile){
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
  if(!dataFile){
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
  if(!dataFile){
    Serial.println("Error on opening the file");
    return;
  }
  int bitCounter = 8;
  int regCounter = 0;
  byte shiftReg[imageWidthBytes] = {0};
  byte temp;
  while(dataFile.available()){
    temp = dataFile.read();
    if(temp == '0'){
      bitCounter--;
      shiftReg[regCounter] = shiftReg[regCounter] << 1;
    }
    else if(temp == '1'){
      bitCounter--;
      shiftReg[regCounter] = (shiftReg[regCounter] << 1) + 1;
    }
    if(bitCounter == 0){
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

void loop() {
  // put your main code here, to run repeatedly:
}

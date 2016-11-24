#include <SPI.h>
#include <SD.h>

//CS pin for Arduino Uno
const int chipSelect = 8;
//const int chipSelect = 4; //for Ethernet shield
const int imageWidthBytes = 8;
const int lineWidth = 65;

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
  int lines = dataFile.size() / lineWidth;
  int bitCounter = 8;
  int regCounter = 0;
  int lineCounter = 0;
  byte shiftReg[lines][imageWidthBytes] = {0};
  byte temp;
  while(lineCounter < lines){
    temp = dataFile.read();
    if(temp == '0'){
      bitCounter--;
      shiftReg[lineCounter][regCounter] = shiftReg[lineCounter][regCounter] << 1;
    }
    else if(temp == '1'){
      bitCounter--;
      shiftReg[lineCounter][regCounter] = (shiftReg[lineCounter][regCounter] << 1) + 1;
    }
    if(bitCounter == 0){
      bitCounter = 8;
      regCounter++;
    }
    if(regCounter == 8){
      regCounter = 0;
      lineCounter++;
    }
  }
  dataFile.close();
  int i, j;
  for(i = 0; i < lines; i++){
    for(j = 0; j < imageWidthBytes; j++){
      Serial.print(j);
      Serial.print(':');
      Serial.print(shiftReg[i][j], BIN);
      Serial.print('\t');
    }
    Serial.print('\n');
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}

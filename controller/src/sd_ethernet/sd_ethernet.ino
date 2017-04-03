
#include <SPI.h>
#include <SD.h>

byte *shiftReg;

//CS pin for Arduino Uno'
const int chipSelect = 8;
//CS pin for Teensy 3.5
//const int chipSelect = BUILTIN_SDCARD;
const int imageWidthBytes = 1;
const int lineWidth = 9;

//////////////////////

void setup(){
  Serial.begin(9600); 
  Serial.println("Initializing SD card...");

  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");
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

void convertToByte(char* filename){
  File dataFile = SD.open(filename);
  if(!dataFile){
    Serial.println("Error on opening the file");
    return;
  }
  int lines = dataFile.size() / lineWidth;
  int total_size = lines * imageWidthBytes;
  shiftReg = (byte*)malloc(total_size * sizeof(byte));
  memset(shiftReg, 0, sizeof(byte) * total_size);
  int bitCounter = 8;
  int regCounter = 0;
  int lineCounter = 0;
  int lineIndex = (lines - 1) * imageWidthBytes;
  int startIndex = lineIndex;
  byte temp;
  while(lineCounter < lines){
    temp = dataFile.read();
    if(temp == '0'){
      bitCounter--;
      shiftReg[lineIndex + regCounter] = shiftReg[lineIndex + regCounter] << 1;
    }
    else if(temp == '1'){
      bitCounter--;
      shiftReg[lineIndex + regCounter] = (shiftReg[lineIndex + regCounter] << 1) + 1;
    }
    if(bitCounter == 0){
      bitCounter = 8;
      regCounter++;
    }
    //change number of regCounter when width is different
    if(regCounter == imageWidthBytes){
      regCounter = 0;
      lineCounter++;
      lineIndex = startIndex - lineCounter * imageWidthBytes;
    }
  }
  dataFile.close();
  int i, j;
  for(i = 0; i < lines; i++){
    for(j = 0; j < imageWidthBytes; j++){
      Serial.print(j);
      Serial.print(':');
      int index = i * imageWidthBytes + j;
      Serial.print(shiftReg[index], BIN);
      Serial.print('\t');
    }
    Serial.print('\n');
  }
  Serial.print('\n');
}

void loop(){
  convertToByte("1.txt");
} 

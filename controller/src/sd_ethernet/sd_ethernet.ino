
#include <SPI.h>
#include <SD.h>

byte *shiftReg;

//CS pin for Arduino Uno'
const int chipSelect = 8;
//CS pin for Teensy 3.5
//const int chipSelect = BUILTIN_SDCARD;
const int imageWidthBytes = 1;
const int lineWidth = 9;
int loopCounter = 1;
int pushbutton[6] = {2, 3, 4, 5, 6, 7};
int buttonState = 0;

//////////////////////

void setup(){
  Serial.begin(9600); 
  Serial.println("Initializing SD card...");

  if(!SD.begin(chipSelect)){
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("Card initialized");

  int i;
  for(i = 0; i < 6; i++){
    pinMode(pushbutton[i], INPUT);
  }
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

char *lookupTable(int index){
  char *filename;
  switch(index){
    case 1:
      filename = "1.txt";
      break;
    case 2:
      filename = "2.txt";
      break;
    case 3:
      filename = "3.txt";
      break;
    case 4:
      filename = "4.txt";
      break;
    case 5:
      filename = "5.txt";
      break;
    case 6:
      filename = "6.txt";
      break;
    default:
      Serial.println("wrong number entered");
  }
  return filename;
}

void loadSelectedPattern(int index){
  int i;
  for(i = 0; i < 5; i++){
    convertToByte(lookupTable(index));
  }
}

void loop(){
  int i;
  for(i = 0; i < 6; i++){
    buttonState = digitalRead(pushbutton[i]);
    if(buttonState == LOW){
      delay(20000);
      loadSelectedPattern(pushbutton[i]-1);
      delay(20000);
      break;
    }
  }
  
  convertToByte(lookupTable(loopCounter));
  loopCounter++;
  if(loopCounter > 6){
    loopCounter = 1;
  }
} 

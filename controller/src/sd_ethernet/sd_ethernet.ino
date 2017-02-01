#include <SPI.h>
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

//byte shiftReg[16][2] = {0};
byte *shiftReg;

int current_pattern_line = 0;

void update_output(void)
{
  //Update the output with the data stored in the shift register
  digitalWrite(RCLK_PIN, HIGH);
  digitalWrite(RCLK_PIN, LOW); 

  //Update the data stored in the shift register to be ready for the next line as soon as the timer triggers
  current_pattern_line = (current_pattern_line + 1) % PATTERN_LENGTH;
  
  //Iterate through the 8 bits being sent to a shift register for a given row (prepare the next row for output)
  int j;
  for (j = 0; j < VALVES_PER_VALVE_BANK; j++)
  {
    //Iterate through the shift registers, sending one bit to each
    int k;
    for (k = 0; k < NUM_VALVE_BANKS; k++)
    {
      if ((shiftReg[(current_pattern_line * NUM_VALVE_BANKS) + k] >> j) & 1)
        digitalWrite(serial_pins[k], HIGH);
      else
        digitalWrite(serial_pins[k], LOW);
    }
    //Write this set of bits into the shift registers (1 per shift register)
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }
}

void setup_shiftregs() {
  MsTimer2::set(250, (&update_output)); // initialize timer to trigger update_output(), and set a 250 ms period
  MsTimer2::start();

  digitalWrite(SRCLR_PIN, HIGH); //Disable Serial Clear
  digitalWrite(OE_PIN, LOW); //Enable output
}
// END OF CODE FOR CONTROLLING THE SHIFT REGISTERS AND LEDS  //
//////////////////////////////////////////////////////////////////////////////

//CS pin for Arduino Uno
const int chipSelect = 8;
//const int chipSelect = 4; //for Ethernet shield
//const int imageWidthBytes = 8;
const int imageWidthBytes = 2;
const int lineWidth = 17;

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

void checkCard(){
  boolean flag = SD.exists("twitter.txt");
  if(flag){
    Serial.println("File exists");
  }else{
    Serial.println("File does not exist");
  }
}

void convertToByte(){
  File dataFile = SD.open("twitter.txt");
  if(!dataFile){
    Serial.println("Error on opening the file");
    return;
  }
  int lines = dataFile.size() / lineWidth;
  int total_size = lines * imageWidthBytes;
  memset(shiftReg, 0, sizeof(byte) * total_size);
  //int lines = 16;
  int bitCounter = 8;
  int regCounter = 0;
  int lineCounter = 0;
  int lineIndex = 0;
  //byte shiftReg[lines][imageWidthBytes] = {0};
  byte temp;
  while(lineCounter < lines){
    temp = dataFile.read();
    if(temp == '0'){
      bitCounter--;
      //shiftReg[lineCounter][regCounter] = shiftReg[lineCounter][regCounter] << 1;
      shiftReg[lineIndex + regCounter] = shiftReg[lineIndex + regCounter] << 1;
    }
    else if(temp == '1'){
      bitCounter--;
      //shiftReg[lineCounter][regCounter] = (shiftReg[lineCounter][regCounter] << 1) + 1;
      shiftReg[lineIndex + regCounter] = (shiftReg[lineIndex + regCounter] << 1) + 1;
    }
    if(bitCounter == 0){
      bitCounter = 8;
      regCounter++;
    }
    //change number of regCounter when width is different
    if(regCounter == 2){
      regCounter = 0;
      lineCounter++;
      lineIndex = lineCounter * imageWidthBytes;
    }
  }
  dataFile.close();
  int i, j;
  for(i = 0; i < lines; i++){
    for(j = 0; j < imageWidthBytes; j++){
      Serial.print(j);
      Serial.print(':');
      //Serial.print(shiftReg[i][j], BIN);
      int index = i * imageWidthBytes + j;
      Serial.print(shiftReg[index], BIN);
      Serial.print('\t');
    }
    Serial.print('\n');
  }
}

void loop(){
  //convertToByte();
  setup_shiftregs();
  while(1);
} 

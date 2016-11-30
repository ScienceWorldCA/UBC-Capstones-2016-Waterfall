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

byte shiftReg[PATTERN_LENGTH][NUM_VALVE_BANKS] ={{0b00000000,0b00000000},
                                                {0b11111111,0b11111111},
                                                {0b00000000,0b00000000},
                                                {0b11111111,0b11111111},
                                                {0b00000000,0b00000000},
                                                {0b01010101,0b01010101},
                                                {0b10101010,0b10101010},
                                                {0b01010101,0b01010101},
                                                {0b10101010,0b10101010},
                                                {0b00110011,0b00110011},
                                                {0b11001100,0b11001100},
                                                {0b00110011,0b00110011},
                                                {0b11001100,0b11001100},
                                                {0b11111111,0b00000000},
                                                {0b00000000,0b11111111},
                                                {0b00001111,0b11110000}};
                
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
      if ((shiftReg[current_pattern_line][k] >> j) & 1)
        digitalWrite(serial_pins[k], HIGH);
      else
        digitalWrite(serial_pins[k], LOW);
    }
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }
}

void setup() {
  pinMode(SRCLR_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(SER1_PIN, OUTPUT);
  pinMode(SER2_PIN, OUTPUT);
  
  //Timer1.initialize(250000);         // initialize timer1, and set a 1/4 second period
  //Timer1.attachInterrupt(update_output);  // attaches update_output() as a timer overflow interrupt
  MsTimer2::set(250, (&update_output));
  MsTimer2::start();
}

void loop() {
  
  digitalWrite(SRCLR_PIN, HIGH);
  digitalWrite(OE_PIN, LOW);
  
  for(;;);
   
}

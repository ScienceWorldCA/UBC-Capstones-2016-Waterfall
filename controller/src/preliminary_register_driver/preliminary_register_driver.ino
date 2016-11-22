#include "TimerOne.h"

#define SRCLR_PIN 13  //Active low
#define SRCLK_PIN 12
#define RCLK_PIN 11
#define OE_PIN 10     //Active Low

#define VALVES_PER_VALVE_BANK 8
#define NUM_VALVE_BANKS 2
#define NUM_VALVES NUM_VALVE_BANKS * VALVES_PER_VALVE_BANK

//TODO: Likely change this to come from the pattern file
#define PATTERN_LENGTH 16

#define SER1_PIN 8
#define SER2_PIN 9

int serial_pins[NUM_VALVE_BANKS] = {SER1_PIN, SER2_PIN};

char pattern[PATTERN_LENGTH][NUM_VALVES] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                           {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                           {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
                                           {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                                           {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
                                           {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                           {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
                                           {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},
                                           {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},
                                           {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},
                                           {0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},
                                           {1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},
                                           {1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
                                           {0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},
                                           {0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0}};
                
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

void setup() {
  pinMode(SRCLR_PIN, OUTPUT);
  pinMode(SRCLK_PIN, OUTPUT);
  pinMode(RCLK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  pinMode(SER1_PIN, OUTPUT);
  pinMode(SER2_PIN, OUTPUT);
  
  Timer1.initialize(250000);         // initialize timer1, and set a 1/4 second period
  Timer1.attachInterrupt(update_output);  // attaches update_output() as a timer overflow interrupt
}

void loop() {
  
  digitalWrite(SRCLR_PIN, HIGH);
  digitalWrite(OE_PIN, LOW);
  
  for(;;);
   
}

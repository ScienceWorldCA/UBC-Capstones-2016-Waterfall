//#include "valve_output.h"

int previous_pattern_line = 0;
int current_pattern_line = 0;

//counter used for determining when to update output, and for handling delay between writing high->low and low->high transitions
//to account for valve switching time
int output_timer_counter;

byte pattern_load_low_high;
byte pattern_load_high_low;
volatile byte pattern_load_full_line;

void update_output(void)
{
  //do the low->high transition early then tell main to load the high->low transition
  if (output_timer_counter == 0)
  { 
    //Update the output with the data stored in the shift register
    digitalWrite(RCLK_PIN, HIGH);
    digitalWrite(RCLK_PIN, LOW);

    pattern_load_high_low = 1;
    pattern_load_full_line = 1;
  }
  #ifdef OPEN_CLOSE_OFFSET
  //do the high->low transition late then tell main to load the low->high transition 
  else if (output_timer_counter == PIXEL_OFFSET_TIME)
  { 
    //Update the output with the data stored in the shift register
    digitalWrite(RCLK_PIN, HIGH);
    digitalWrite(RCLK_PIN, LOW);

    pattern_load_low_high = 1;
  }
  #endif

  //Reset the counter every PIXEL_TIME interrupts, causes pixel length to be PIXEL_TIME milliseconds
  output_timer_counter = (output_timer_counter + 1) % PIXEL_TIME;
}

//Switching of valves from open to closed must occur earlier due to switching time of valves
void load_shiftreg_low_high (byte *shiftReg, int lineCount)
{
  
  //Update the data stored in the shift register to be ready for the next line as soon as the timer triggers
  current_pattern_line = (current_pattern_line + 1) % lineCount;
  
  
  //Iterate through the 8 bits being sent to a shift register for a given row (prepare the next row for output)
  int j;
  for (j = 0; j < VALVES_PER_VALVE_BANK; j++)
  {
    //Iterate through the shift registers, sending one bit to each
    int k;
    for (k = 0; k < NUM_VALVE_BANKS; k++)
    {
      //If current pattern is switching 0 to 1, 1 to 1, or 1 to 0 set the output to 1 (open to closed, or closed to closed)
      //otherwise, if shifting from 0 to 0, set to 0
      //The 1 to 0 transition is delayed to account for opening/closing time of valves
      
      //Only set low if the previous and current output are both low
      if (!((shiftReg[(current_pattern_line * NUM_VALVE_BANKS) + k] >> j) & 1) && !((shiftReg[(previous_pattern_line * NUM_VALVE_BANKS) + k] >> j) & 1))
        digitalWrite(serial_pins[k], LOW);
      else
        digitalWrite(serial_pins[k], HIGH);
    }
    //Write this set of bits into the shift registers (1 per shift register)
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }

  pattern_load_low_high = 0;
}

//Switching of valves from closed to open must occur later due to switching time of valves
void load_shiftreg_high_low (byte *shiftReg)
{
  //Update the data stored in the shift register to be ready for the next line as soon as the timer triggers
  
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
  //Update the previous_pattern_line to contain the current line
  previous_pattern_line = current_pattern_line;

  pattern_load_high_low = 0;
}

void load_shiftreg_full_line (byte *shiftReg, int lineCount){
  //Update the data stored in the shift register to be ready for the next line as soon as the timer triggers

  current_pattern_line = (current_pattern_line + 1) % lineCount;
  
  //Iterate through the 8 bits being sent to a shift register for a given row (prepare the next row for output)
  int j;
  for (j = 0; j < VALVES_PER_VALVE_BANK; j++)
  {
    //Iterate through the shift registers, sending one bit to each
    int k;
    for (k = 0; k < NUM_VALVE_BANKS; k++)
    {
      digitalWrite(serial_pins[k], (shiftReg[(current_pattern_line * NUM_VALVE_BANKS) + k] >> j) & 1);
    }
    //Write this set of bits into the shift registers (1 per shift register)
    digitalWrite(SRCLK_PIN, HIGH);
    delay(10);
    digitalWrite(SRCLK_PIN, LOW);
    delay(10);
  }

  pattern_load_full_line = 0;
}

void setup_shiftregs() {
  output_timer_counter = 0;
  previous_pattern_line = NULL;

  //Set the timer interrupt to trigger every millisecond
  MsTimer2::set(1, (&update_output)); // initialize timer to trigger update_output(), and set a 250 ms period
  MsTimer2::start();

  digitalWrite(SRCLR_PIN, HIGH); //Disable Serial Clear
  digitalWrite(OE_PIN, LOW); //Enable output
}

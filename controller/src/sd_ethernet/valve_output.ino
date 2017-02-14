#include "valve_output.h"

byte *previous_pattern_line;
int current_pattern_line = 0;

//counter used for determining when to update output, and for handling delay between writing high->low and low->high transitions
//to account for valve switching time
int output_timer_counter;

//TODO: combine into one flag
byte pattern_load_high_low;
byte pattern_load_low_high;

void update_output(void)
{
  //do the high->low transition early then tell main to load the low->high transition
  if (output_timer_counter == 0)
  {
    Serial.println("High->low\r\n");
    
    //Update the output with the data stored in the shift register
    digitalWrite(RCLK_PIN, HIGH);
    digitalWrite(RCLK_PIN, LOW);

    pattern_load_low_high = 1;
  }
  //do the low->high transition early then tell main to load the high->low transition 
  else if (output_timer_counter == PIXEL_OFFSET_TIME)
  {
    Serial.println("Low->High\r\n");
    
    //Update the output with the data stored in the shift register
    digitalWrite(RCLK_PIN, HIGH);
    digitalWrite(RCLK_PIN, LOW);

    pattern_load_high_low = 1;
  }

  //Reset the counter every PIXEL_TIME interrupts, causes pixel length to be PIXEL_TIME milliseconds
  output_timer_counter = (output_timer_counter + 1) % PIXEL_TIME;
}

//Switching of valves from open to closed must occur earlier due to switching time of valves
void load_shiftreg_high_low (byte *shiftReg)
{
  
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
      //If current output bit is 1 AND previous output bit is 1, output 1 again
      //Otherwise, if value is staying 0 or switching 1 to 0 output 0
      //If we are switching high to low, delay this output to account for valve switching time
      if ((shiftReg[(current_pattern_line * NUM_VALVE_BANKS) + k] >> j) & 1 && (previous_pattern_line[k] >> j) & 1)
        digitalWrite(serial_pins[k], HIGH);
      else
        digitalWrite(serial_pins[k], LOW);
    }
    //Write this set of bits into the shift registers (1 per shift register)
    digitalWrite(SRCLK_PIN, HIGH);
    digitalWrite(SRCLK_PIN, LOW);
  }

  pattern_load_high_low = 0;
}

//Switching of valves from closed to open must occur later due to switching time of valves
void load_shiftreg_low_high (byte *shiftReg)
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
  previous_pattern_line = &shiftReg[current_pattern_line];

  pattern_load_low_high = 0;
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

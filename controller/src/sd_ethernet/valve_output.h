#ifndef _VALVE_OUTPUT_H_
#define _VALVE_OUTPUT_H_

//Pin definitions for valve output shift registers
#define SRCLR_PIN 9  //Active low
#define SRCLK_PIN 8
#define RCLK_PIN 7
#define OE_PIN 6     //Active Low

#define SER1_PIN 2
#define SER2_PIN 3

//TODO: Likely change this to come from the pattern file
#define PATTERN_LENGTH 6

//Definitions for output size
//Modify these values for larger implementations with more valve banks
#define VALVES_PER_VALVE_BANK 8
#define NUM_VALVE_BANKS 1
#define NUM_VALVES NUM_VALVE_BANKS * VALVES_PER_VALVE_BANK

//length of each output "pixel" in milliseconds
#define PIXEL_TIME 100

//The time to delay the writing of low->high transitions (in ms)
#define PIXEL_OFFSET_TIME 25

//static const int serial_pins[NUM_VALVE_BANKS] = {SER1_PIN, SER2_PIN};
static const int serial_pins[NUM_VALVE_BANKS] = {SER1_PIN};

extern byte pattern_load_high_low;
extern byte pattern_load_low_high;

void setup_shiftregs(void);
void load_shiftreg_low_high (byte *);
void load_shiftreg_high_low (byte *);
void update_output(void);


#endif

README for Digital Waterfall driver software

=================================================================================

SECTIONS:
1) DESCRIPTION OF PROGRAM FLOW
2) MODIFYING PERFORMANCE
3) MODIFYING FOR DIFFERENT NUMBER OF VALVES

=================================================================================

1) DESCRIPTION OF PROGRAM FLOW
-SD card initialized
-Input/Output modes set for IO
-Load an initial image
-Setup valve output timer and button debounce timer
-Loop Forever:
  -Check if button is pressed and load associate pattern
  -If button was pressed, wait the debounce time before checking buttons again
  -If flag set by interrupt, load next pattern line into shift register

  -On timer interrupt:
    -Toggle RCLK of shift registers, writing data to their outputs
    -Set flag indicating main loop to load next line of data into shift registers

=================================================================================

2) MODIFYING PERFORMANCE
a) Tuning Pixel Length
To tune the length of a single pixel, modify the value stored in the #define PIXEL_TIME statement in valve_output.h. This is the time in milliseconds that a valve will remain open or closed for a single line of the pattern. A smaller value will lead to faster switching and pixels of shorter length.

b) Enabling Pixel Length Normalization
During a valve's switching time, water flows making it look open. As a result, the "open" bits appear longer than the "closed" bits. To combat this, logic has been implemented to cause all open -> closed transitions to happen earlier than the closed -> open transitions. This helps to even out bit sizes and prevent overlapping of bits.
To enable this offsetting mode, uncomment the #define OPEN_CLOSE_OFFSET statement in valve_output.h

c) Tuning Pixel Length Normalization
To adjust the amount of offset used in the Pixel Length Normalization, modify the #define PIXEL_OFFSET_TIME statement in valve_output.h. This value represents the time in milliseconds between the occurence of the open -> closed switching and the closed -> open switching. If using Pixel Length Normalization, tis value must be smaller than PIXEL_TIME to ensure proper operation

=================================================================================

3) MODIFYING FOR DIFFERENT NUMBER OF VALVES
Whe modifying for a different number of valves, there are a few definitions that need to change. Originally, this system uses 3 banks of 8 valves each. It is recommended that you not modify the number of valves in a single bank for several reasons. 8 valves per bank makes it easy to split bank data into bytes. Also, the shift registers used in this project have 8 bits and therefore work well as part of an 8 valve bank.

a) Changing the number of valve banks
-Modify the value of NUM_VALVE_BANKS defined in valve_output.h
-Modify the value of imageWidthBytes defined in waterfall.ino
-Modify the value of lineWidth defined in waterfall.ino to be (number of valve -banks * 8) + 1
-The pattern files also need to change to support the different valve count



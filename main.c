 // CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  


#include <xc.h>
#include "color.h"
#include "serial.h"
#include "dc_motor.h"
#include "interrupt.h"
#include "home.h"
#include "timers.h"


void main(void) {
    Interrupts_init();
    Timer0_interrupt_init();

    // Declare motor structs
    DC_motor motorL, motorR;
    motorStruct(&motorL, &motorR);

    // Other initializations    
    initBuggyLights();
    initButtons();   
    color_click_init();
    initUSART4();

    // Declare HSV struct
    HSV color;

    // Declare timeStack and commandStack structs
    Stack timeStack, commandStack;
    initialiseStack(&timeStack, &commandStack);

    char go = 0;
    char calibrate = 0;
    extern volatile unsigned int overflowCount; // Global declaration
        
    while (1) {
        // Calibration Routine
        if (!PORTFbits.RF3) { // Detect button press for calibration
            calibrate = 1; 
        }

        if (calibrate) {
            turnRight135(&motorL, &motorR);
            calibrate = 0;
        }

        // Maze Solving Routine
        if (!PORTFbits.RF2) { // Detect button press to start maze solving
            go = 1;
            Timer0_init();
            overflowCount = 0;
        }

        if (go) {
            setGoLED();
            fullSpeedAhead(&motorL, &motorR);

            // Read ambient light level and set threshold for obstacle detection
            unsigned int clearVal = color_read_Clear();
            unsigned int clearThreshold = (90 * clearVal) / 100;

            if (color_read_Clear() < clearThreshold) { // Obstacle Detected
                stop(&motorL, &motorR); // Stop motors

                // Record the time spent moving forward
                push(&timeStack, overflowCount);

                // Align with wall and read color
                turnOffLEDs();
                wallAlign(&motorL, &motorR);
                HSV color = ReadHSV();
                char command = ClassifyColor(color);

                // Execute the command
                CommandBuggy(&motorL, &motorR, command, 1);

                // Reset overflow count for the next forward movement
                overflowCount = 0;

                if (command == WHITE || command == LOST) { // If white or lost, return home
                    goHome(&motorL, &motorR, &timeStack, &commandStack);

                    // Stop motors, arrived home
                    go = 0;
                    stop(&motorL, &motorR);
                    turnOffLEDs();
                    break;
                } else {
                    // Push the flipped command onto the commandStack for the return home
                    push(&commandStack, flipCommand(command));
                }
            }
        }
    }
}

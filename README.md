# Course project - Mine navigation search and rescue

## Demo Video

A video of the buggy solving a maze is provided.

https://github.com/user-attachments/assets/5d4b0d59-9a30-4f14-992d-d6a7459a622b


## Addressing Point 1
To accurately measure ambient lighting, the buggy records the initial light level when button RF2 is pressed. Additionaly , after completing every maneuver, the buggy re-measures the ambient light in its current direction to ensure an up-to-date value.

A threshold is then calculated based on this ambient light reading. When the light intensity exceeds 93% of this threshold (this value can be changed depending on lighting conditions), the buggy halts and initiates its color reading sequence.

	unsigned int clearVal = color_read_Clear();
	unsigned int clearThreshold = (93 * clearVal) / 100;
	
	if (color_read_Clear() < clearThreshold) {  // Detect obstacle
	// stop motors
	stop(&motorL, &motorR);

To align the buggy with the wall precisely, it moves forward at a reduced speed. This slow, controlled movement allows the buggy to position itself flush against the wall, improving the consistency of color readings and correcting any minor path deviations from previous turns. 

	 //align and read color
		turnOffLEDs();
		wallAlign(&motorL, &motorR); 
		color = ReadHSV();
		char command = ClassifyColor(color);
	
## Addressing Point 2
To read the color card the Tricolor LED are flashed in RGB sequence and the corresponding channels are read, the clear channel is also read for normalising the RGB values. 10 readings are taken from each channel and averaged to ensure the normalised RGB values are consistent, An example is shown for reading the red channel. Double data types are used in order to keep the precision of the values before they are further processed.

    // Measure Red Channel
    double totalR = 0.0;
    for (char i = 0; i < 10; i++) {
        flash_red();
        __delay_ms(50);
        totalR += (double)color_read_Red();
    }

The normalised RGB values are converted to HSV color space in the ReadHSV() function in color.c. This transformation aids in better color boundary separation, and the resulting HSV values are stored in an HSV struct for subsequent calculations
<p align="center">
  <img src="gifs/RGBtoHSV.png" width="600" height="350">
</p>

	// Normalise RGB values using Clear channel and scale
	double R = (avgC > 0) ? (avgR / avgC) * SCALE_FACTOR : 0;
	double G = (avgC > 0) ? (avgG / avgC) * SCALE_FACTOR : 0;
	double B = (avgC > 0) ? (avgB / avgC) * SCALE_FACTOR : 0;
	
	double max = fmax(fmax(R, G), B);
	double min = fmin(fmin(R, G), B);
	double delta = max - min;
	
	HSV hsv;
	
	// Calculate Hue (H)
	if (delta == 0) {
	    hsv.H = 0;
	} else if (max == R) {
	    hsv.H = (int)(60.0 * fmod(((G - B) / delta), 6.0));
	} else if (max == G) {
	    hsv.H = (int)(60.0 * (((B - R) / delta) + 2.0));
	} else {
	    hsv.H = (int)(60.0 * (((R - G) / delta) + 4.0));
	}
	
	if (hsv.H < 0) {
	    hsv.H += 360;
	}
	
	// Calculate Saturation (S)
	hsv.S = (max == 0) ? 0 : (int)((delta / max) * 100.0);
	
	// Calculate Value (V)
	hsv.V = (int)((max / SCALE_FACTOR) * 100.0);
	
	return hsv;
    
Through rigorous data collection and testing, it was found that calculating the ratio of Saturation/Hue provided a robust method for threshold-based color classification. However, some pale colors, such as Yellow, Pink, White, and Light Blue, presented challenges due to overlapping ranges. To mitigate this issue, additional conditions based on hue and saturation values were introduced, effectively separating these colors. A flow chart of the process is shown:   
<p align="center">
  <img src="gifs/Color classify.png" width="1000" height="500">
</p>

ClassifyColor() function (in color.c): 

	char ClassifyColor(HSV hsv) {
	    
	    // Calculate S/H ratio using ternary operator, scaling S by 1000 to avoid numerical losses
	    double S_H_ratio = (hsv.H != 0) ? ((double)hsv.S * 1000) / hsv.H : 0;
	
	    // Color classification using if-else statements with continuous thresholds
	    if (S_H_ratio > 15000) {
	        return 1; //RED
	    } else if (S_H_ratio > 8000 && S_H_ratio <= 15000) {
	        return 2; // ORANGE
	    } else if (S_H_ratio > 2900 && S_H_ratio <= 8000 && hsv.S > 85) { // Saturation to handle overlap with PINK
	        return 3; //YELLOW
	    } else if (S_H_ratio > 900 && S_H_ratio <= 3200) {
	        // Nested if statement for PINK, WHITE, and LIGHT BLUE 
	        if (hsv.H < 35) {
	            return 4; // PINK
	        } else if (hsv.H >= 35 && hsv.H <= 45) {
	            return 5; // WHITE
	        } else {
	            return 6; //LIGHT BLUE
	        }
	    } else if (S_H_ratio > 800 && S_H_ratio <= 900) {
	        return 7; // GREEN
	    } else if (S_H_ratio <= 800) {
	        return 8; // BLUE
	    } else {
	        return 0; // LOST (unrecognised color)
	    }
	}
 
## Addressing Point 3
The buggy receives instructions based on the color detected by the CommandBuggy function in dc_motor.c. The mode parameter distinguishes between maze-solving mode and returning home mode (1 for maze solving, 0 for returning home). In returning home mode, longer reverse movements associated with the Yellow and Pink colors are adjusted to forward movements:

	// Function to command motors based on color and mode
	void CommandBuggy(DC_motor *mL, DC_motor *mR, char color , char mode) {
	    if (color == 1) { // RED, turn right
	        shortReverse(mL, mR);
	        turnRight(mL, mR);
	    } else if (color == 2) { // ORANGE, turn right 135
	        shortReverse(mL, mR);
	        turnRight135(mL, mR);
	    } else if (color == 3) { // YELLOW, reverse 1 square and turn right
	        if (mode == 1) {
	        longReverse(mL, mR);
	        turnRight(mL, mR);
	        }  else {
	            turnRight(mL, mR);
	            fullSpeedAhead(mL, mR);
	            __delay_ms(reverseTime);
	            stop(mL, mR);
	        }
	    } else if (color == 4) { // PINK, reverse 1 square and turn left 
	        if (mode == 1) {
	        longReverse(mL, mR);
	        turnLeft(mL, mR);
	        }  else {
	            turnLeft(mL, mR);
	            fullSpeedAhead(mL, mR);
	            __delay_ms(reverseTime);
	            stop(mL, mR);
	        }
	    } else if (color == 5) { // WHITE, turn 180 then go home
	        shortReverse(mL, mR);
	        turn180(mL, mR);
	    } else if (color == 6) { // LIGHT BLUE, turn left 135
	        shortReverse(mL, mR);
	        turnLeft135(mL, mR);
	    } else if (color == 7) { // GREEN, turn left
	        shortReverse(mL, mR);
	        turnLeft(mL, mR);
	    } else if (color == 8) { // BLUE, turn 180
	        shortReverse(mL, mR);
	        turn180(mL, mR);
	    } else {
	        // Default action if LOST
	        // Lost, turn 180 and go home
	        shortReverse(mL, mR);
	        turn180(mL, mR);       
	    }
	}
 
 The turning times need to be calibrated depending on the friction of the floor where the buggy is being run, this is done by setting the delay for defined turn times in dc_motor.h:
 
	# define turnTimeleft 500// values determined from calibration 
	# define turnTimeright 420
	# define reverseTime 2700
	# define turnTimeLeft135 500
	# define turnTimeRight135 550
	# define turnTime180 1300
 
## Addressing Point 4
To enable the buggy to remember its past actions, two stacks are used: one to record the flipped commands (the opposite of the commands executed in maze-solving mode) and the other to track the time spent moving forward. The stack data structure is employed because it follows a "last in, first out" (LIFO) principle, allowing the buggy to effectively retain memory of its actions.
<p align="center">
  <img src="gifs/Stack.png" width="700" height="350">
</p>
In main.c once the color read is white (or lost) the function goHome(), located in home.c, is called and the buggy will turn 180 and return to its starting position:

	void goHome(DC_motor *mL, DC_motor *mR, Stack *timeStack, Stack *commandStack) {
	    // Check if timeStack is empty
	     while (!isEmpty(timeStack)){
	
	        setGoLED();
	        setCalibrationLED();
	
	        int lastTime = pop(timeStack);

	        if (lastTime > 0) { // Ensure lastTime is valid
	            setGoLED();
	            fullSpeedAhead(mL, mR);
	
	            // Reset overflowCount and wait for the equivalent time
	            overflowCount = 0;
	            while (overflowCount < lastTime) {
	                sendUnsignedIntSerial4(overflowCount);
	                // Wait until the timer has counted the required duration
	            }
	
	            stop(mL, mR);
	            turnOffLEDs();
	        }
	
	        if (!isEmpty(commandStack)) {
	            char lastCommand = pop(commandStack);
	            CommandBuggy(mL, mR, lastCommand);
	        }
	    }
	
	    stop(mL, mR);
	    turnOffLEDs();
	    return;
	}
Accurate timing is maintained using the Timer 0 module, where a predefined value is loaded into the TMR0L and TMR0H registers to trigger an interrupt every 10 ms. When the interrupt occurs, the global variable overflowCount (declared in interrupt.h) is incremented. If the buggy detects an obstacle, the current value of overflowCount is pushed onto the time stack. This ensures precise timing for each forward stretch.

## Addressing Point 5
The color black, representing maze walls, does not fall within any thresholds for color reading. As a result, the ClassifyColor() function returns 0 when the buggy is misaligned and detects the color of a maze wall. In this case, the same action as for the white color is executed: the buggy performs a 180-degree turn and returns home.

# Maze-navigating-robot
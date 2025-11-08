## 🧭 Maze-Solving Robot – Mine Navigation Search & Rescue
This project presents an autonomous maze-solving robot designed for a mine search and rescue scenario.
The robot uses color-coded cards as navigational cues and makes decisions using a trained decision tree classifier implemented directly in C.
The system combines:
- Embedded firmware for sensing, control, and navigation
- On-board color-based decision logic
- Machine learning–based color classification

## 🎥 Demo Video
Watch the robot autonomously navigate a maze and return to its starting position:

https://github.com/user-attachments/assets/5d4b0d59-9a30-4f14-992d-d6a7459a622b


## 🌍 System Overview
The robot explores a maze by following light cues and color cards placed strategically along the path.
Each color card represents a command (turn, reverse, or stop), and the robot interprets it in real-time.
When a white card is detected, the robot recognizes the maze’s exit and automatically returns to its starting point using stored movement history.

## 💡 Ambient Light Calibration
To adapt to changing lighting conditions, the robot first measures ambient brightness when the user presses button RF2.
It continuously re-measures ambient light after each maneuver to maintain up-to-date thresholds.
	
	unsigned int clearVal = color_read_Clear();
	unsigned int clearThreshold = (93 * clearVal) / 100;
	
	if (color_read_Clear() < clearThreshold) {  // Detect obstacle
	    stop(&motorL, &motorR);
	}

The robot then aligns itself precisely with the wall and performs a controlled color-reading sequence for maximum accuracy.

	turnOffLEDs();
	wallAlign(&motorL, &motorR); 
	color = ReadHSV();
	char command = ClassifyColor(color); 

## 🎨 Color Detection and Processing

To identify the color cards, Tri-color LEDs are flashed sequentially (R → G → B), and corresponding sensor channels are read.
Ten readings per channel are averaged to minimize noise and improve consistency.

	double totalR = 0.0;
	for (char i = 0; i < 10; i++) {
	    flash_red();
	    __delay_ms(50);
	    totalR += (double)color_read_Red();
	}

After capturing RGB data, the readings are normalized and converted to HSV color space, which allows for better separation between similar colors.
<p align="center"> <img src="gifs/RGBtoHSV.png" width="600" height="350"> </p>
	
	// Normalise RGB values using Clear channel
	double R = (avgC > 0) ? (avgR / avgC) * SCALE_FACTOR : 0;
	double G = (avgC > 0) ? (avgG / avgC) * SCALE_FACTOR : 0;
	double B = (avgC > 0) ? (avgB / avgC) * SCALE_FACTOR : 0;

Through experimentation, a Saturation/Hue ratio was found to be an effective discriminant for color classification.
To handle overlap between pale tones (e.g. yellow, pink, light blue), extra conditions were added based on hue and saturation values.

<p align="center"> <img src="gifs/Color classify.png" width="1000" height="500"> </p>

The resulting classifier:
	char ClassifyColor(HSV hsv) {
	    double S_H_ratio = (hsv.H != 0) ? ((double)hsv.S * 1000) / hsv.H : 0;
	
	    if (S_H_ratio > 15000) return 1; // RED
	    else if (S_H_ratio > 8000) return 2; // ORANGE
	    else if (S_H_ratio > 2900 && hsv.S > 85) return 3; // YELLOW
	    else if (S_H_ratio > 900) {
	        if (hsv.H < 35) return 4; // PINK
	        else if (hsv.H <= 45) return 5; // WHITE
	        else return 6; // LIGHT BLUE
	    } else if (S_H_ratio > 800) return 7; // GREEN
	    else if (S_H_ratio <= 800) return 8; // BLUE
	    else return 0; // LOST
	}
## 🧭 Navigation Logic

Each color card corresponds to a specific movement instruction handled by the CommandBuggy() function:
	
	void CommandBuggy(DC_motor *mL, DC_motor *mR, char color , char mode) {
	    if (color == 1) turnRight(mL, mR);      // RED
	    else if (color == 2) turnRight135(mL, mR); // ORANGE
	    else if (color == 3) longReverse(mL, mR);  // YELLOW
	    else if (color == 4) turnLeft(mL, mR);     // PINK
	    else if (color == 5) turn180(mL, mR);      // WHITE
	    else if (color == 6) turnLeft135(mL, mR);  // LIGHT BLUE
	    else if (color == 7) turnLeft(mL, mR);     // GREEN
	    else if (color == 8) turn180(mL, mR);      // BLUE
	}
	
Timing values for each maneuver are calibrated in dc_motor.h depending on surface friction:
	
	#define turnTimeLeft 500
	#define turnTimeRight 420
	#define reverseTime 2700
	#define turnTimeLeft135 500
	#define turnTimeRight135 550
	#define turnTime180 1300

## 🧱 Memory and Path Tracking
To enable autonomous return-to-home functionality, the robot uses two stack data structures:
One stack stores inverse commands
Another stores forward movement durations
<p align="center"> <img src="gifs/Stack.png" width="700" height="350"> </p>
When a white (exit) card is detected, goHome() is triggered:
	
	void goHome(DC_motor *mL, DC_motor *mR, Stack *timeStack, Stack *commandStack) {
	    while (!isEmpty(timeStack)){
	        int lastTime = pop(timeStack);
	        if (lastTime > 0) {
	            fullSpeedAhead(mL, mR);
	            while (overflowCount < lastTime) {}
	            stop(mL, mR);
	        }
	        if (!isEmpty(commandStack)) {
	            char lastCommand = pop(commandStack);
	            CommandBuggy(mL, mR, lastCommand);
	        }
	    }
	    stop(mL, mR);
	}
Timer interrupts increment a global overflowCount variable every 10 ms, allowing precise tracking of travel time between maneuvers.

## 🚧 Handling Obstacles
Walls (black surfaces) are detected automatically — since their readings don’t match any known color range,
the classifier returns 0 (LOST). In this case, the robot performs a 180° turn and returns home.

## 🤖 Summary
- Component	Description
- Sensors	Color sensor with tricolor LED illumination
- Processing	Onboard microcontroller running C firmware
- Navigation	Color-coded maze commands
- Memory	Stack-based path storage for return navigation
- ML Integration	Decision tree classifier exported to C
- Control Timer-based precise motion and calibrated turns

## 🧠 Future Work
Integrate full decision tree–based classification for more complex color schemes
Improve robustness under changing light conditions
Add real-time serial telemetry for debugging and visualization

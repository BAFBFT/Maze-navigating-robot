#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

# define turnTimeleft 500// values determined from calibration 
# define turnTimeright 420
# define reverseTime 2500
# define turnTimeLeft135 750 // values determined from calibration 
# define turnTimeRight135 700
# define turnTime180 900 //ideal is 900


typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//struct prototypes
void motorStruct(DC_motor *motorL, DC_motor *motorR);

/***********************************************
 *  Structure Initialization Prototype
 ***********************************************/
void motorStruct(DC_motor *motorL, DC_motor *motorR);

/***********************************************
 *  Function Prototypes
 ***********************************************/

/**
 * Initialises PWM for DC motors.
 * @param PWMperiod Base period of the PWM cycle.
 */
void initDCmotorsPWM(int PWMperiod);

/**
 * Initialises the buggy lights.
 */
void initBuggyLights(void);

/**
 * Initialises input buttons.
 */
void initButtons(void);

/**
 * Activates the "Go" LED indicator.
 */
void setGoLED(void);

/**
 * Activates the calibration LED indicator.
 */
void setCalibrationLED(void);

/**
 * Turns off all LED indicators.
 */
void turnOffLEDs(void);

/**
 * Sets the PWM duty cycle for the specified motor.
 * @param m Pointer to the DC_motor structure.
 */
void setMotorPWM(DC_motor *m);

/**
 * Stops both motors.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void stop(DC_motor *mL, DC_motor *mR);

/**
 * Turns the buggy left.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void turnLeft(DC_motor *mL, DC_motor *mR);

/**
 * Turns the buggy right.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void turnRight(DC_motor *mL, DC_motor *mR);

/**
 * Executes a 180-degree turn.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void turn180(DC_motor *mL, DC_motor *mR);

/**
 * Moves the buggy forward at full speed.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void fullSpeedAhead(DC_motor *mL, DC_motor *mR);

/**
 * Aligns the buggy.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void align(DC_motor *mL, DC_motor *mR);

/**
 * Reverses the buggy for a short distance.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void shortReverse(DC_motor *mL, DC_motor *mR);

/**
 * Reverses the buggy for a longer distance.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void longReverse(DC_motor *mL, DC_motor *mR);

/**
 * Executes a 135-degree left turn.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void turnLeft135(DC_motor *mL, DC_motor *mR);

/**
 * Executes a 135-degree right turn.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void turnRight135(DC_motor *mL, DC_motor *mR);

/**
 * Aligns the buggy with a wall.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void wallAlign(DC_motor *mL, DC_motor *mR);

/**
 * Executes the calibration routine.
 * @param mL Pointer to the left motor.
 * @param mR Pointer to the right motor.
 */
void calibrationRoutine(DC_motor *mL, DC_motor *mR);

/**
 * Executes a command on the buggy based on color and mode (maze solving = 1).
 * @param motorL Pointer to the left motor.
 * @param motorR Pointer to the right motor.
 * @param color The detected color.
 * @param mode The mode to execute.
 */
void CommandBuggy(DC_motor *mL, DC_motor *mR, char color, char mode);

#endif

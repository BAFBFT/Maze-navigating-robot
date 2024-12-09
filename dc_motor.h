#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

# define turnTimeleft 250// values determined from calibration 
# define turnTimeright 250
# define reverseTime 600
# define turnTimeLeft135 650 // values determined from calibration 
# define turnTimeRight135 600


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

//function prototypes
void initDCmotorsPWM(int PWMperiod); // function to setup PWM
void initBuggyLights(void);
void initButtons(void);
void setGoLED(void);
void setCalibrationLED(void);
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
void shortReverse(DC_motor *mL, DC_motor *mR);
void longReverse(DC_motor *mL, DC_motor *mR);
void turnLeft135(DC_motor *mL, DC_motor *mR);
void turnRight135(DC_motor *mL, DC_motor *mR);
void wallAlign(DC_motor *mL, DC_motor *mR);
void CommandBuggy(DC_motor *motorL, DC_motor *motorR, char color);

#endif

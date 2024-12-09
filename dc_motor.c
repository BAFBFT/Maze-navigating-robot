#include <xc.h>
#include "dc_motor.h"
#include "color.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    TRISEbits.TRISE2 = 0;
    LATEbits.LATE2 = 0;
    TRISEbits.TRISE4 = 0;
    LATEbits.LATE4 = 0;
    TRISCbits.TRISC7 = 0;
    LATCbits.LATC7 = 0;
    TRISGbits.TRISG6 = 0;
    LATGbits.LATG6 = 0; 
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b011; // 1:8 prescaler (125 kHz)
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=199; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

void initBuggyLights(void)
{
    //turns headlights on
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;
    
    //initialise left turn signals
    TRISFbits.TRISF0 = 0;
    LATFbits.LATF0 = 0;
    
    //initialise right turn signals
    TRISEbits.TRISE5 = 0;
    LATEbits.LATE5 = 0;
    
    //initialise brake turn signals
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 0;
    
    //initialise LED D7
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7= 0;
    
    //initialise LED D7
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
}

void initButtons(void)
{
    //initialise buttons
    TRISFbits.TRISF2 = 1;
    ANSELFbits.ANSELF2 = 0;
    TRISFbits.TRISF3 = 1;
    ANSELFbits.ANSELF3 = 0;
}

//function to turn on go LED
void setGoLED(void){
    LATDbits.LATD7 = 1;
}

//function to turn on calibration LED
void setCalibrationLED(void){
    LATHbits.LATH3 = 1;
}

// function to turn off LEDs
void turnOffLEDs(void) {
    LATHbits.LATH3 = 0;
    LATDbits.LATD7 = 0;
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

//function to initialise motor struct
void motorStruct(DC_motor *motorL, DC_motor *motorR)
{
    initDCmotorsPWM(199);
    // Initialise motorL
    motorL->power = 0;
    motorL->direction = 1;
    motorL->brakemode = 1;
    motorL->posDutyHighByte = (unsigned char *)(&CCPR1H);
    motorL->negDutyHighByte = (unsigned char *)(&CCPR2H);
    motorL->PWMperiod = 199;

    // Initialise motorR
    motorR->power = 0;
    motorR->direction = 1;
    motorR->brakemode = 1;
    motorR->posDutyHighByte = (unsigned char *)(&CCPR3H);
    motorR->negDutyHighByte = (unsigned char *)(&CCPR4H);
    motorR->PWMperiod = 199;
}

//function to stop the robot gradually 
void stop(DC_motor *mL, DC_motor *mR)
{
    LATFbits.LATF7 = 1; //turn on brake lights
    mL->power=0; //set Left wheel to stop
    mL->brakemode = 1; //cause breaking effect
    mR->power=0; //set Right wheel to stop
    mR->brakemode = 1; //cause breaking effect
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);   
    
    LATFbits.LATF7 = 0; //turn off brake lights
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    LATFbits.LATF0 = 1; //signal left
    mL->power=50; //set Left wheel to go 50%
    mL->direction=0; //set Left wheel direction to reverse
    mR->power=50; //set Right wheel to go 50%
    mR->direction=1; //set Right wheel direction to forward
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(turnTimeleft); //allow time to turn
    stop(mL, mR); //then stop
    LATFbits.LATF0 = 0; //stop signaling
}

//function to make the robot turn right 
void turnRight(DC_motor *mL, DC_motor *mR)
{
    LATEbits.LATE5 = 1; //signal right
    mL->power=50; //set Left wheel to go 50%
    mL->direction=1; //set Left wheel direction to forward
    mR->power=50; //set Right wheel to go 50%
    mR->direction=0; //set Right wheel direction to reverse
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(turnTimeright); //allow time to turn
    stop(mL, mR); //then stop
    LATEbits.LATE5 = 0; //stop signalling
}

//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    mL->power=30; //set Left wheel to go 50%
    mL->direction=1; //set Left wheel direction to forward
    mR->power=30; //set Right wheel to go 50%
    mR->direction=1; //set Right wheel direction to forward
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
}

//function to make the robot go a short distance in reverse
void shortReverse(DC_motor *mL, DC_motor *mR)
{
    mL->power= 50; //set Left wheel to go power set in .h
    mL->direction= 0; //set Left wheel direction to forward
    mL->brakemode = 1; //cause breaking effect
    mR->power= 50; //set Right wheel to go power set in .h
    mR->direction= 0; //set Left wheel direction to forward
    mR->brakemode = 1; //cause breaking effect
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(250);
    stop(mL, mR); //then stop
    __delay_ms(100);
}

//function to make the robot go 1 box in reverse
void longReverse(DC_motor *mL, DC_motor *mR)
{
    mL->power= 50; //set Left wheel to go power set in .h
    mL->direction= 0; //set Left wheel direction to forward
    mL->brakemode = 1; //cause breaking effect
    mR->power= 50; //set Right wheel to go power set in .h
    mR->direction= 0; //set Left wheel direction to forward
    mR->brakemode = 1; //cause breaking effect
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(reverseTime);
    stop(mL, mR); //then stop
    __delay_ms(100);
}

// function to make the  robot turn left 135
void turnLeft135(DC_motor *mL, DC_motor *mR)
{
    LATEbits.LATE5 = 1; //signal right
    mL->power=50; //set Left wheel to go 50%
    mL->direction=0; //set Left wheel direction to forward
    mR->power=50; //set Right wheel to go 50%
    mR->direction=1; //set Right wheel direction to reverse
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(turnTimeLeft135); //allow time to turn
    stop(mL, mR); //then stop
    LATEbits.LATE5 = 0; //stop signalling
}

// function to make the  robot turn right 135
void turnRight135(DC_motor *mL, DC_motor *mR)
{
    LATEbits.LATE5 = 1; //signal right
    mL->power=50; //set Left wheel to go 50%
    mL->direction=1; //set Left wheel direction to forward
    mR->power=50; //set Right wheel to go 50%
    mR->direction=0; //set Right wheel direction to reverse
    
    setMotorPWM(mL); //send to controller
    setMotorPWM(mR);
    
    __delay_ms(turnTimeRight135); //allow time to turn
    stop(mL, mR); //then stop
    LATEbits.LATE5 = 0; //stop signalling
}

// function to make the robot realign with the wall
void wallAlign(DC_motor *mL, DC_motor *mR){
    char j = 0;
    while (j < 5) {
        fullSpeedAhead(mL, mR);
        __delay_ms(150);
        j++;
    }
    // ensure motor stops
    stop(mL, mR); 
}

//function to test turn calibration
void calibrationRoutine(DC_motor *mL, DC_motor *mR){
    turnLeft(mL, mR);
    __delay_ms(500);
    turnRight(mL, mR);
    __delay_ms(500);
    turnLeft135(mL, mR);
    __delay_ms(500);
    turnRight135(mL, mR);
    __delay_ms(500);
    shortReverse(mL, mR);
    __delay_ms(500);
    longReverse(mL, mR);
}

//Function to command motors based on color
void CommandBuggy(DC_motor *motorL, DC_motor *motorR, char color) {
    if (color == 1) {
        shortReverse(motorL, motorR);
        turnRight(motorL, motorR);
    } else if (color == 2) {
        shortReverse(motorL, motorR);
        turnRight135(motorL, motorR);
    } else if (color == 3) {
        longReverse(motorL, motorR);
        turnRight(motorL, motorR);
    } else if (color == 4) {
        longReverse(motorL, motorR);
        turnLeft(motorL, motorR);
    } else if (color == 5) {
        shortReverse(motorL, motorR);
        for (char i = 0; i < 2; i++) {
            turnLeft(motorL, motorR);
            __delay_ms(500); // Wait 500 ms between turns
        }
        // white, turn 180, go home
    } else if (color == 6) {
        shortReverse(motorL, motorR);
        turnLeft135(motorL, motorR);
    } else if (color == 7) {
        shortReverse(motorL, motorR);
        turnLeft(motorL, motorR);
    } else if (color == 8) {
        shortReverse(motorL, motorR);
        for (char i = 0; i < 2; i++) {
            turnLeft(motorL, motorR);
            __delay_ms(500); // Wait 500 ms between turns
        }
    } else {
        // Default action if no valid color classification
        stop(motorL, motorR); // Define stopMotors to halt both motors
    }
}

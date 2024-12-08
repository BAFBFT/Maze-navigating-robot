#include <xc.h>
#include <stdint.h>
#include <math.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    //setup tricolor_led
    TRISGbits.TRISG1 = 0;//red is AN 
    LATGbits.LATG1 = 0;
    TRISAbits.TRISA4 = 0;//green is CS 
    LATAbits.LATA4 = 0;
    TRISFbits.TRISF7 = 0;//blue is PWM
    LATFbits.LATF7 = 0;
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Clear(void)
{
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at CLEAR low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp = I2C_2_Master_Read(1);			//read the Clear LSB
	tmp = tmp | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp = I2C_2_Master_Read(1);			//read the Red LSB
	tmp = tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at GREEN low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp = I2C_2_Master_Read(1);			//read the Green LSB
	tmp = tmp | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
    unsigned int tmp;
    I2C_2_Master_Start();
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at BLUE low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp = I2C_2_Master_Read(1);			//read the Blue LSB
	tmp = tmp | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

void flash_red(void)
{
    LATGbits.LATG1 = 1;
    LATAbits.LATA4 = 0;
    LATFbits.LATF7 = 0;
}

void flash_green(void)
{
    LATGbits.LATG1 = 0;
    LATAbits.LATA4 = 1;
    LATFbits.LATF7 = 0;
}

void flash_blue(void)
{
    LATGbits.LATG1 = 0;
    LATAbits.LATA4 = 0;
    LATFbits.LATF7 = 1;
}

void turn_off_LED(void)
{
    LATGbits.LATG1 = 0;
    LATAbits.LATA4 = 0;
    LATFbits.LATF7 = 0;
}


HSV ReadHSV(void) {
    const double SCALE_FACTOR = 100.0;
    
    // Measure Red Channel
    double totalR = 0.0;
    for (char i = 0; i < 10; i++) {
        flash_red();
        __delay_ms(50);
        totalR += (double)color_read_Red();
    }
    double avgR = totalR / 10.0;
    turn_off_LED();

    // Measure Green Channel
    double totalG = 0.0;
    for (char i = 0; i < 10; i++) {
        flash_green();
        __delay_ms(50);
        totalG += (double)color_read_Green();
    }
    double avgG = totalG / 10.0;
    turn_off_LED();

    // Measure Blue Channel
    double totalB = 0.0;
    for (char i = 0; i < 10; i++) {
        flash_blue();
        __delay_ms(50);
        totalB += (double)color_read_Blue();
    }
    double avgB = totalB / 10.0;
    turn_off_LED();

    // Measure Clear Channel
    double totalC = 0.0;
    for (char i = 0; i < 10; i++) {
        __delay_ms(50);
        totalC += (double)color_read_Clear();
    }
    double avgC = totalC / 10.0;

    // Normalize RGB values using Clear channel and scale
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
}


char ClassifyColor(HSV hsv) {
    
    // Calculate S/H ratio, scaling S by 1000
    double S_H_ratio = (hsv.H != 0) ? ((double)hsv.S * 1000) / hsv.H : 0;

    // Color classification using if-else statements with continuous thresholds
    if (S_H_ratio > 20000) {
        return 1; //RED
    } else if (S_H_ratio > 9000 && S_H_ratio <= 20000) {
        return 2; // ORANGE
    } else if (S_H_ratio > 2900 && S_H_ratio <= 9000 && hsv.S > 85) { // Saturation to handle overlap with PINK
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
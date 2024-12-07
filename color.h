#ifndef _color_H
#define _color_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define SCALE 1024  // Scaling factor to preserve precision
 

/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Red(void);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Blue(void);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Green(void);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Clear(void);


void flash_red(void);
void flash_green(void);
void flash_blue(void);
void turn_off_LED(void);

typedef struct RGBC{
    float R; // Red
    float G; // Green
    float B; // Blue
    float C; // Clear
} RGBC;

typedef struct HSV{
    unsigned int H; // Hue
    unsigned int S; // Saturation
    unsigned int V; // Value
} HSV;

//const char* classify_color(RGBC color);
//const char* classifyColor(uint16_t R, uint16_t G, uint16_t B, uint16_t C);
//HSV RGBtoHSV(RGBC rgbc);
HSV ReadHSV(void);
RGBC MeasureRGBC(void);
const char* ClassifyColor(HSV hsv);
#endif

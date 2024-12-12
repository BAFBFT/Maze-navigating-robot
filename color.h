#ifndef _color_H
#define _color_H

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define SCALE 1024  // Scaling factor to preserve precision
#define WHITE 5 // Command value for white
#define LOST 0 // Command value for unclassified colors
 

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
 *  Reads the clear (ambient light) channel intensity
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
unsigned int color_read_Clear(void);

/***********************************************
 *  Flashes the red LED 
 ***********************************************/
void flash_red(void);

/***********************************************
 *  Flashes the green LED 
 ***********************************************/
void flash_green(void);

/***********************************************
 *  Flashes the blue LED 
 ***********************************************/
void flash_blue(void);

/***********************************************
 *  Turns off all LED indicators
 ***********************************************/
void turn_off_LED(void);

/***********************************************
 *  Structure representing HSV color values
 ***********************************************/
typedef struct HSV {
    unsigned int H; // Hue
    unsigned int S; // Saturation
    unsigned int V; // Value
} HSV;

/***********************************************
 *  Reads HSV color values from the color sensor
 *  @return HSV structure containing Hue, Saturation, and Value
 ***********************************************/
HSV ReadHSV(void);

/***********************************************
 *  Classifies a given HSV value into a color category
 *  @param hsv HSV structure containing color data
 *  @return Encoded color category
 ***********************************************/
char ClassifyColor(HSV hsv);
#endif

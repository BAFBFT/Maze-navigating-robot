#include <xc.h>
#include <stdint.h>
#include "color.h"
#include "i2c.h"

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



RGBC MeasureRGBC(void) {
    // Initialise RGBC struct to hold normalized red, green, blue, and clear channel values
    RGBC rgbc;

    // Measure Red Channel
    unsigned long totalR = 0; //use unsigned long to avoid truncation 
    for (char i = 0; i < 10; i++) {
        flash_red();
        __delay_ms(50);
        totalR += color_read_Red();
    }
    unsigned long avgR = totalR / 10;  // Average the red channel readings
    turn_off_LED();

    // Measure Green Channel
    unsigned int totalG = 0;
    for (char i = 0; i < 10; i++) {
        flash_green();
        __delay_ms(50);
        totalG += color_read_Green();
    }
    unsigned long avgG = totalG / 10;  // Average the green channel readings
    turn_off_LED();

    // Measure Blue Channel
    unsigned long totalB = 0;
    for (char i = 0; i < 10; i++) {
        flash_blue();
        __delay_ms(50);
        totalB += color_read_Blue();
    }
    unsigned long avgB = totalB / 10;  // Average the blue channel readings
    turn_off_LED();

    // Measure Clear Channel
    unsigned long totalC = 0;
    for (char i = 0; i < 10; i++) {
        __delay_ms(50);
        totalC += color_read_Clear();
    }
    unsigned long avgC = totalC / 10;  // Average the clear channel readings


    rgbc.R = (avgR * SCALE) / avgC;  // Normalise and scale red
    rgbc.G = (avgG * SCALE) / avgC;  // Normalise and scale green
    rgbc.B = (avgB * SCALE) / avgC;  // Normalise and scale blue
    rgbc.C = avgC * SCALE;                 // Normalise clear to the scaling factor


    // Return the populated RGBC struct containing normalised values
    return rgbc;
}


// Function to classify the color
const char* classifyColor(uint16_t R, uint16_t G, uint16_t B, uint16_t C) {
    // Prevent division by zero
    if (C == 0) {
        C = 1;
    }

    // Scale normalized RGB values to integers
    uint32_t R_norm_int = ((uint32_t)R * SCALE) / C;
    uint32_t G_norm_int = ((uint32_t)G * SCALE) / C;
    uint32_t B_norm_int = ((uint32_t)B * SCALE) / C;

    // Decision tree for classification
    if (R_norm_int > 6000 && G_norm_int < 4000 && B_norm_int < 4000) {
        return "Red";
    } else if (G_norm_int > 6000 && R_norm_int < 4000 && B_norm_int < 4000) {
        return "Green";
    } else if (B_norm_int > 6000 && R_norm_int < 4000 && G_norm_int < 4000) {
        return "Blue";
    } else if (R_norm_int > 5000 && G_norm_int > 5000 && B_norm_int < 3000) {
        return "Yellow";
    } else if (R_norm_int > 6000 && B_norm_int > 4000 && G_norm_int < 4000) {
        return "Pink";
    } else if (R_norm_int > 5000 && G_norm_int > 3000 && B_norm_int < 2000) {
        return "Orange";
    } else if (B_norm_int > 5000 && G_norm_int > 5000 && R_norm_int < 4000) {
        return "Light Blue";
    } else if (R_norm_int > 7000 && G_norm_int > 7000 && B_norm_int > 7000) {
        return "White";
    } else if (R_norm_int < 2000 && G_norm_int < 2000 && B_norm_int < 2000) {
        return "Black";
    } else {
        return "Unknown";
    }
}
                
                
HSV RGBtoHSV(RGBC rgbc) {
    
    //Obtain readings from sensor
    unsigned int R = rgbc.R;
    unsigned int G = rgbc.G;
    unsigned int B = rgbc.B;
    unsigned int C = rgbc.C;
    
    // Normalise RGB values
    unsigned int R_norm = (R * SCALE) / C;
    unsigned int G_norm = (G * SCALE) / C;
    unsigned int B_norm = (B * SCALE) / C;

    // Find max and min
    unsigned int max, min, delta;

    // Determine the maximum value
    if (R_norm > G_norm) {
        if (R_norm > B_norm) {
            max = R_norm;  // R_norm is the largest
        } else {
            max = B_norm;  // B_norm is larger than R_norm
        }
    } else {
        if (G_norm > B_norm) {
            max = G_norm;  // G_norm is the largest
        } else {
            max = B_norm;  // B_norm is larger than G_norm
        }
    }

    // Determine the minimum value
    if (R_norm < G_norm) {
        if (R_norm < B_norm) {
            min = R_norm;  // R_norm is the smallest
        } else {
            min = B_norm;  // B_norm is smaller than R_norm
        }
    } else {
        if (G_norm < B_norm) {
            min = G_norm;  // G_norm is the smallest
        } else {
            min = B_norm;  // B_norm is smaller than G_norm
        }
    }

    // Calculate delta
    delta = max - min;
    
    //initialise HSV struct
    HSV hsv;
    
    // Calculate Value (V)
    hsv.V = (max * 100) / SCALE;

    // Calculate Saturation (S)    
    if (max == 0){
        hsv.S = 0; // 0 saturation when value is 0
    } else {
        hsv.S = (delta * 100) / max;
    }

    // Calculate Hue (H)
    if (delta == 0) {
        hsv.H = 0; // Undefined hue
    } else if (max == R_norm) {
        hsv.H = ((60 * (G_norm - B_norm) / delta) + 360) % 360;
    } else if (max == G_norm) {
        hsv.H = ((60 * (B_norm - R_norm) / delta) + 120) % 360;
    } else {
        hsv.H = ((60 * (R_norm - G_norm) / delta) + 240) % 360;
    }
    
    return hsv;
}
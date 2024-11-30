#ifndef _SERIAL_H
#define _SERIAL_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define RX_BUF_SIZE 20
#define TX_BUF_SIZE 60

//basic EUSART funcitons
void initUSART4(void);
void sendCharSerial4(char charToSend);
void sendStringSerial4(char *string);

#endif

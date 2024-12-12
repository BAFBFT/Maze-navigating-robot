#ifndef _SERIAL_H
#define _SERIAL_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

#define RX_BUF_SIZE 20
#define TX_BUF_SIZE 60

/***********************************************
 *  Initializes the EUSART4 module
 ***********************************************/
void initUSART4(void);

/***********************************************
 *  Sends a single character over EUSART4
 *  @param charToSend The character to be sent
 ***********************************************/
void sendCharSerial4(char charToSend);

/***********************************************
 *  Sends a null-terminated string over EUSART4
 *  @param string Pointer to the string to send
 ***********************************************/
void sendStringSerial4(char *string);

/***********************************************
 *  Sends an unsigned integer as ASCII characters over EUSART4
 *  @param value The unsigned integer to send
 ***********************************************/
void sendUnsignedIntSerial4(unsigned int value);
#endif

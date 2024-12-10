#ifndef _home_H
#define _home_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  
#define _I2C_CLOCK 100000 //100kHz for I2C

#define MAX_SIZE 100

// Define a structure for the stack
typedef struct Stack {
    // Array to store stack elements
    int arr[MAX_SIZE];  
    // Index of the top element in the stack
    int top;        
} Stack;

void initialiseStack(Stack *timeStack, Stack *commandStack);
char isEmpty(Stack *stack);
char isFull(Stack *stack);
void push(Stack *stack, int value);
char flipCommand(char color);
int pop(Stack *stack);
void goHome(DC_motor *mL, DC_motor *mR, Stack *timeStack, Stack *commandStack);
#endif
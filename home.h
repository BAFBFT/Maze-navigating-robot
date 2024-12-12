#ifndef _home_H
#define _home_H

#include <xc.h>
#include "dc_motor.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  
#define _I2C_CLOCK 100000 //100kHz for I2C

// Max size of the stack
#define MAX_SIZE 100

/***********************************************
 *  Structure representing a stack
 ***********************************************/
typedef struct Stack {
    int arr[MAX_SIZE];  // Array to store stack elements
    int top;            // Index of the top element in the stack
} Stack;

/***********************************************
 *  Initializes the stack structures
 *  @param timeStack Pointer to the stack for storing timing data
 *  @param commandStack Pointer to the stack for storing commands
 ***********************************************/
void initialiseStack(Stack *timeStack, Stack *commandStack);

/***********************************************
 *  Checks if the stack is empty
 *  @param stack Pointer to the stack to check
 *  @return 1 if the stack is empty, 0 otherwise
 ***********************************************/
char isEmpty(Stack *stack);

/***********************************************
 *  Checks if the stack is full
 *  @param stack Pointer to the stack to check
 *  @return 1 if the stack is full, 0 otherwise
 ***********************************************/
char isFull(Stack *stack);

/***********************************************
 *  Pushes a value onto the stack
 *  @param stack Pointer to the stack
 *  @param value Value to be pushed onto the stack
 ***********************************************/
void push(Stack *stack, int value);

/***********************************************
 *  Flips a given command to its opposite
 *  @param color Encoded color command
 *  @return Flipped command
 ***********************************************/
char flipCommand(char color);

/***********************************************
 *  Pops a value from the stack
 *  @param stack Pointer to the stack
 *  @return Value popped from the stack
 ***********************************************/
int pop(Stack *stack);

/***********************************************
 *  Executes the routine to return the buggy home
 *  @param mL Pointer to the left motor structure
 *  @param mR Pointer to the right motor structure
 *  @param timeStack Pointer to the stack storing timing data
 *  @param commandStack Pointer to the stack storing commands
 ***********************************************/
void goHome(DC_motor *mL, DC_motor *mR, Stack *timeStack, Stack *commandStack);

#endif

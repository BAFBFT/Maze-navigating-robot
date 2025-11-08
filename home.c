#include <xc.h>
#include "home.h"
#include <stdio.h>
#include "dc_motor.h"
#include "interrupt.h"
#include "serial.h"

//function to initialise motor struct
void initialiseStack(Stack *timeStack, Stack *commandStack)
{
    // Initialise timeStack
    timeStack->top = -1;

    // Initialise commandStack
    commandStack->top = -1;
}

// Function to check if the stack is empty
char isEmpty(Stack *stack) {
    // If top is -1, the stack is empty
    if (stack->top == -1) {
        return 1;
    } else {
        return 0;
    }
}

// Function to check if the stack is full
char isFull(Stack *stack) {
    // If top is MAX_SIZE - 1, the stack is full
    if (stack->top == MAX_SIZE - 1) {
        return 1;
    } else {
        return 0;
    }
}

// Function to push an element onto the stack
void push(Stack *stack, int value) {
    // Check for stack overflow
    if (isFull(stack) == 1) {
        return;
    }
    // Increment top and add the value to the top of the stack
    stack->arr[++stack->top] = value;
}

// Function to pop an element from the stack
int pop(Stack *stack) {

    // Check for stack underflow
    if (isEmpty(stack) == 1) {
        return -1;
    }
    
    // Return the top element 
    int popped = stack->arr[stack->top];
    // decrement top pointer
    stack->top--;
    // return the popped element
    return popped;
}

// Function to flip the color command for return home
char flipCommand(char color){
    
    if (color == 1) { // Right <==> Left
        return 7;
    } else if (color == 2) { // Right135 <==> Left135
        return 6;
    } else if (color == 3) { // Reverse-Right <==> Reverse-Left
        return 4;
    } else if (color == 4) { // Reverse-Left <==> Reverse-Right
        return 3;
    }  else if (color == 6) { // Left135  <==> Right135
        return 2;
    } else if (color == 7) { // Left <==> Right
        return 1;
    } else { // Turn 180
        return 8;
    }
}

// Function to return the buggy to its starting position
void goHome(DC_motor *mL, DC_motor *mR, Stack *timeStack, Stack *commandStack) {
    // Process all movements stored in the time stack
    while (!isEmpty(timeStack)) {
        // Indicate active operation with LEDs
        setGoLED();
        setCalibrationLED();

        // Retrieve the last recorded time from the stack
        int lastTime = pop(timeStack);
        if (lastTime > 0) { // Proceed only if the time value is valid
            setGoLED();
            fullSpeedAhead(mL, mR);

            // Wait for the duration of the last recorded movement
            overflowCount = 0;
            while (overflowCount < lastTime);

            // Align with the wall and stop
            wallAlign(mL, mR);
            stop(mL, mR);
            __delay_ms(500);
            turnOffLEDs();
        }

        // Process the last recorded command, if available
        if (!isEmpty(commandStack)) {
            char lastCommand = pop(commandStack);
            CommandBuggy(mL, mR, lastCommand, 0);
        }
    }

    // Stop the buggy and turn off LEDs upon completion
    stop(mL, mR);
    turnOffLEDs();
}


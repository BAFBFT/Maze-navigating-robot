/* 
 * File:   interrupt.h
 * Author: aj1322
 *
 * Created on 30 November 2024, 14:46
 */

#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>

#define _XTAL_FREQ 64000000

// Global variable to keep track of timing; incremented in the Timer0 interrupt handler
volatile unsigned int overflowCount = 0;

/************************************
 *  Initializes the interrupt system
************************************/
void Interrupts_init(void);

/************************************
 *  Configures Timer0 to generate periodic interrupts.
 *  This includes setting up the timer registers and enabling the Timer0 interrupt.
************************************/
void Timer0_interrupt_init(void);

/************************************
 * High-priority interrupt service routine.
 * Handles Timer0 overflow interrupts to manage precise timing.
 * Increments overflowCount
************************************/
void __interrupt(high_priority) HighISR_timer();


#endif
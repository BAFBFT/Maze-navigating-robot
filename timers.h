#ifndef _timers_H
#define _timers_H

#include <xc.h>

#define _XTAL_FREQ 64000000

/************************************
 * Initializes Timer0 for operation.
 * Configures the timer registers, prescaler, and preload values 
 * to ensure precise timing.
************************************/
void Timer0_init(void);

#endif

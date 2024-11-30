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

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();

#endif
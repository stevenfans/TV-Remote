// SysTick.h
// Runs on LM4F120/TM4C123
// Provide functions that initialize the SysTick module, wait at least a
// designated number of clock cycles, and wait approximately a multiple
// of 10 milliseconds using busy wait.  After a power-on-reset, the
// LM4F120 gets its clock from the 16 MHz precision internal oscillator,
// which can vary by +/- 1% at room temperature and +/- 3% across all
// temperature ranges.  If you are using this module, you may need more
// precise timing, so it is assumed that you are using the PLL to set
// the system clock to 50 MHz.  This matters for the function
// SysTick_Wait10ms(), which will wait longer than 10 ms if the clock is
// slower.
// Daniel Valvano
// September 11, 2013

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
   Program 2.11, Section 2.6
 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "waveforms.h"

void SysTick_Init(unsigned long period){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
	NVIC_ST_RELOAD_R = period-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x40000000; // priority 2
  NVIC_ST_CTRL_R = 0x07;      // enable SysTick with core clock and interrupts
	
}

// Time delay using busy wait.
// The delay parameter is in units of the core clock. (units of 20 nsec for 50 MHz clock)
void SysTick_Wait(uint32_t delay){
  volatile uint32_t elapsedTime;
  uint32_t startTime = NVIC_ST_CURRENT_R;
  do{
    elapsedTime = (startTime-NVIC_ST_CURRENT_R)&0x00FFFFFF;
  }
  while(elapsedTime <= delay);
}

// Time delay using busy wait.
// This assumes 50 MHz system clock.
void SysTick_Wait10ms(uint32_t delay){
  uint32_t i;
  for(i=0; i<delay; i++){
    SysTick_Wait(500000);  // wait 10ms (assumes 50 MHz clock)
  }
}

unsigned long frequency_count = 0;
extern int packet[5];
extern int input_device_number, current_device_number, previous_current_device_number;
void SysTick_Handler(){
	frequency_count++;
	
	if(input_device_number==current_device_number){
			if (packet[2]== 1 && packet[3]==0 && packet[4] == 0){
				sineWave(frequency_count);
				previous_current_device_number  = 4;
				//GPIO_PORTF_DATA_R = 0x0A;
			}
			else if (packet[2]== 1 && packet[3]==0 && packet[4] == 1){
				squareWave(frequency_count);
				previous_current_device_number  = 5;
				//GPIO_PORTF_DATA_R = 0x0E; //white
				}
			else if (packet[2]== 1 && packet[3]==1 && packet[4] == 0){
				triangleWave(frequency_count);
				previous_current_device_number  = 6;
				//GPIO_PORTF_DATA_R = 0x06; //pink
			}
			else if (packet[2]== 1 && packet[3]==1 && packet[4] == 1){
				//GPIO_PORTF_DATA_R = 0x0C; // skyblue
				previous_current_device_number  = 7;
				sawtoothWave(frequency_count);
			}
			else{
				//GPIO_PORTF_DATA_R = 0x00;
				clearWave();
			}
		}
	else{
		if (previous_current_device_number  == 4) sineWave(frequency_count);
		if (previous_current_device_number  == 5) squareWave(frequency_count);
		if (previous_current_device_number  == 6) triangleWave(frequency_count);
		if (previous_current_device_number  == 7) sawtoothWave(frequency_count);
	}
	
	if (frequency_count == 255) frequency_count = 0;
}




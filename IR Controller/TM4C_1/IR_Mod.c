// IR communcation using port A
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "IR_Mod.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))
	
void Init_PortA(void){ 
  SYSCTL_RCGC2_R |= 0x00000001;     // 1) A clock
  GPIO_PORTA_CR_R |= 0x0C;          // 2) allow changes to PA3, PA2  0000.1100
  GPIO_PORTA_AMSEL_R &= ~0xFF;      // 3) disable analog function
  GPIO_PORTA_PCTL_R &= ~0xFF;  		  // 4) GPIO clear bit PCTL  
  GPIO_PORTA_DIR_R = 0x0C;        	// 5) PB3-2 output 
  GPIO_PORTA_AFSEL_R &= ~0xFF;      // 6) no alternate function
  GPIO_PORTA_DEN_R |= 0x0C;         // 7) enable digital pins PA3-2   
}

void modulateSignal(void){
	SIGNAL = 0xFF;
	SysTick_Wait19Khz(); 
	SIGNAL = 0x00; 
	SysTick_Wait19Khz(); 
}
void nomodulateSignal(void){
	SysTick_Wait19Khz(); 
	SysTick_Wait19Khz(); 
}
// 1ms high = 38 pulses,  800us low = 30 pulses
void startPulse(void){
	unsigned long i; 
	for(i=0; i<38; i++){
		modulateSignal(); 
	}
	for(i=0; i<30; i++){
		nomodulateSignal(); 
	}
}

void logical_1(void){
	unsigned long i; 
	for(i=0; i<34; i++){
		modulateSignal(); 
	}
	for(i=0; i<17; i++){
		nomodulateSignal(); 
	}
}
void logical_0(void){
	unsigned long i; 
	for(i=0; i<17; i++){
		modulateSignal(); 
	}
	for(i=0; i<17; i++){
		nomodulateSignal(); 
	}
}
	
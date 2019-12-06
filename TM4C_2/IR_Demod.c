// IR communcation using port A 
// used to demodulate a signal 
// with a running clock of 50 Mhz
#include "SysTick.h"
#include "IR_Demod.h"
#include "tm4c123gh6pm.h"
#include "IR_Demod.h"
#include "UART.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))

void Init_PortA(void){ 
	unsigned long delay; 
	SYSCTL_RCGC2_R |= 0x00000001; 
	delay = SYSCTL_RCGC2_R; 
	GPIO_PORTA_DATA_R |= 0x04; 				//use PA2
	GPIO_PORTA_AFSEL_R &= ~0x04; 		//disable all alt funciton
	GPIO_PORTA_DEN_R |= 0x04; 				// used this pins 
	GPIO_PORTA_DIR_R &= ~0x04; 				// input pins on PA5~0
	GPIO_PORTA_CR_R |= 0x04;          // allow changes to PD5-0      
  GPIO_PORTA_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTA_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL 
	
	GPIO_PORTA_IS_R &= ~0x04;
	GPIO_PORTA_IBE_R &= ~0x04;
	GPIO_PORTA_IEV_R &= ~0x04;
	GPIO_PORTA_ICR_R = 0x04;
	GPIO_PORTA_IM_R |= 0x04;
	NVIC_PRI0_R = (NVIC_PRI0_R&0xFFFFFF0F) | 0x000000E0;
	NVIC_EN0_R |= 0x00000001;
}

void GPIOPortA_Handler(void){

	if (GPIO_PORTA_RIS_R & 0x04){ // sw1 is pressed
		GPIO_PORTA_ICR_R = 0x04; // clear flag
		GPIO_PORTF_DATA_R = 0x02; 
	}
}


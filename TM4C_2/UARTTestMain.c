#include "PLL.h"
#include "UART.h"
#include "Tone.h"
#include "SysTick.h"
#include "tm4c123gh6pm.h"
#include "IR_Demod.h"

#define SIGNAL       				(*((volatile unsigned long *)0x40004030))

int running = 0; 
unsigned int tally_1 = 0; // hold the count for every time there is a 1 
unsigned int tally_0 = 0; // hold the count for every time there is a 0

void PORTF_Init(void);
//---------------------OutCRLF---------------------
// Output a CR,LF to UART to go to a new line
// Input: none
// Output: none
void OutCRLF(void){
  UART_OutChar(CR);
  UART_OutChar(LF);
}

int main(void){
	unsigned long frequency = 262;
	unsigned long last_frequency = 262;
	unsigned long freq_value;  
	char string [10]; 
	
	PLL_Init();
  UART_Init();
	UART2_Init();
	
	Init_PortA(); 
	Init_PortB();
	PORTF_Init();
	SysTick_Init(800); 
	GPIO_PORTF_DATA_R = 0x0E; 
	UART_OutChar('a'); 
  while(1){
		
		UART_OutString("Signal 0 Count: "); UART_OutUDec(tally_0);
		UART_OutString("    Signal 1 Count: "); UART_OutUDec(tally_1);
		OutCRLF(); 
		if(tally_0>1){
			GPIO_PORTF_DATA_R = 0x02; 
		}
		
  }
}

void PORTF_Init(void){      
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) F clock
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0        	
  SYSCTL_RCGC2_R |= 0x00000020; // (a) activate clock for port F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_DIR_R &= ~0x10;    // (c) make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  //     disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x10;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}


// interrupt checks to see the start of the decoding
void GPIOPortA_Handler(void){
	if (GPIO_PORTA_RIS_R & 0x04){ // sw1 is pressed
		GPIO_PORTA_ICR_R = 0x04; // clear flag
		if (running==0){
			running = 1; 
		}
	}
}


void SysTick_Handler(){
	if(running==1){
		GPIO_PORTF_DATA_R = 0x08;
		if (SIGNAL==0) tally_0++; 
		else tally_1++; 
	}
}

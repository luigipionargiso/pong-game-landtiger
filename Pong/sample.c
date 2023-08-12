#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "led/led.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "adc/adc.h"
#include "GLCD/GLCD.h"
#include "TouchPanel/TouchPanel.h"
#include "pong_game/game.h"
 
 #ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();  
	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	ADC_init();
	LCD_Initialization();
	
	init_timer(2, 0, 0, 3, 0xCB735);			/* display refresh rate: 30Hz -> k=0xCB735 */ 
	
	LPC_PINCON->PINSEL1 |= (1<<21);				/* Buzzer Initialization */
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
	
	titleScreen();
	
  while (1) {
		// Loop forever
  }

}

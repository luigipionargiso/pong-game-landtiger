#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../button_EXINT/button.h"
#include "../timer/timer.h"
#include "../TouchPanel/TouchPanel.h"
#include "../pong_game/game.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

int down_INT0=0;
int down_KEY1=0;
int down_KEY2=0;

void RIT_IRQHandler (void)
{						
	/* button management */
	/* INT0 */
	if(down_INT0!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){	/* INT0 pressed */
			down_INT0++;
			reset_RIT();
			switch(down_INT0){
				case 2:
					if(getGameStatus() == GAME_OVER) {		/* restarts the game */
						titleScreen();
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_INT0=0;	
			reset_RIT();
			disable_RIT();		
			NVIC_EnableIRQ(EINT0_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	}
	/* KEY1 */
	if(down_KEY1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			down_KEY1++;
			reset_RIT();
			switch(down_KEY1){
				case 2:
					if(getGameStatus() == START) {				/* starts playing */
						gameInit();	
						
						init_timer(0, 0, 0, 3, 1062);				/* sound */
						enable_timer(0);
						
						setGameStatus(PLAY);
						enable_timer(2);
					}
					else if(getGameStatus() == PAUSE) {		/* restarts playing */
						setGameStatus(PLAY);
						enable_timer(2);
					}
					break;
			}
		}
		else {	/* button released */
			down_KEY1=0;
			reset_RIT();
			disable_RIT();	
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	/* KEY2 */
	if(down_KEY2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY2 pressed */
			down_KEY2++;
			reset_RIT();			
			switch(down_KEY2){
				case 2:
					if(getGameStatus() == PLAY) {
						setGameStatus(PAUSE);
						disable_timer(2);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_KEY2=0;
			reset_RIT();			
			disable_RIT();	
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
			
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
}

/******************************************************************************
**                            End Of File
******************************************************************************/

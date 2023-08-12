#include "lpc17xx.h"
#include "timer.h"
#include "../led/led.h"
#include "../GLCD/GLCD.h"
#include "../pong_game/game.h"
#include "../adc/adc.h"

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

/*
 * k=1/f'*f/n  k=f/(f'*n) k=25MHz/(f'*45)
 *
 * 262Hz	k=2120		c4
 * 294Hz	k=1890		
 * 330Hz	k=1684		
 * 349Hz	k=1592		
 * 392Hz	k=1417		
 * 440Hz	k=1263		
 * 494Hz	k=1125		
 * 523Hz	k=1062		c5
 * 
 */

uint16_t SinTable[45] =
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

void TIMER0_IRQHandler (void)		// BUZZER SOUND
{
	static int ticks = 0;
	
	LPC_DAC->DACR = SinTable[ticks]<<6;
	ticks++;
	
	if(ticks == 45) {
		ticks = 0;
		disable_timer(0);
		reset_timer(0);
	}
	
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER1_IRQHandler (void)
{  	
	
	LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER2_IRQHandler (void)								// DISPLAY REFRESH
{
	if (getGameStatus() == PLAY) {
		ADC_start_conversion();									// move the paddle
		drawNextFrame();												// move the ball and the cpuPaddle
	}
	else if (getGameStatus() == GAME_OVER) {
		disable_timer(2);
	}
	
	LPC_TIM2->IR = 1;			/* clear interrupt flag */
	
  return;
}

void TIMER3_IRQHandler (void)
{
	
	LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/

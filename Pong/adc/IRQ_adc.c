#include "lpc17xx.h"
#include "adc.h"
#include "../led/led.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"
#include "../pong_game/game.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short values[10];
float wma_current;
float wma_last = -10.0;

unsigned char k = 0;
unsigned char count = 0;

float ABS( float n ) {
	if(n<0) return -n;
	return n;
}

void ADC_IRQHandler(void) {
	
	unsigned char i;
	
	values[k] = (LPC_ADC->ADGDR>>11) & 0x1F;												// pick only the 5 most significant bits (max_value=31)	
	
	if(count<10) count++;																						// number of obtained values
	
	wma_current=0;
	
	for(i=0; i<count; i++)																					// compute the Weighted Moving Average of the last 10 values
		wma_current += values[(k+1+i)%count]*(i+1);
	wma_current = wma_current/(count*(count+1)/2.0);
	
  if( ABS(wma_current-wma_last) >= 1.0 && count!=1) {							// check if the wma variation is significant (also discard the first value)
		movePaddle((short)(wma_current*170.0/31.0+5));								// (wma-0) : (31-0) = (paddlePosition-wallThick) : ((MAX_X-paddleWidth-wallThick)-wallThick)
		wma_last = wma_current;
	}
	
	k = (k+1)%10;
	
}

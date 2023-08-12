/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           game.h
** Last modified Date:  
** Last Version:        
** Descriptions:        Prototypes of functions included in the game .c files
** Correlated files:    game.c
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#ifndef __GAME_H
#define __GAME_H

typedef enum {START=0, PLAY, PAUSE, GAME_OVER} GAME_STATUS;

extern void gameInit( void );
extern void titleScreen( void );
extern void drawNextFrame( void );

extern void setGameStatus( GAME_STATUS newStatus );
extern GAME_STATUS getGameStatus( void );

extern void movePaddle( short position );

#endif /* end __GAME_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/

/*
 * AniBike2V7BT_Clock.c
 *
 * Created: 6/15/2011 11:06:08 PM
 *  Author: David
 */ 
#include "AniBike2V7BT_Internal.h"

/*****************************************************************
 * Setting the internal RC clock frequency (2-32 MHz)
 * with 2 MHz resolution.
 *****************************************************************/
void SetClockFreq ( uint8_t freq )
{
	if ( freq == 32 )
	{
		/*  Enable internal 32 MHz ring oscillator and wait until it's
		 *  stable. Divide clock by two with the prescaler C and set the
		 *  32 MHz ring oscillator as the main clock source. 
		 */
		CLKSYS_Enable( OSC_RC32MEN_bm );
		CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
		do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
		CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
		
		CLKSYS_Disable( OSC_PLLEN_bm );
		CLKSYS_Disable( OSC_RC2MEN_bm );
	}
	else
	{
		CLKSYS_Enable( OSC_RC2MEN_bm );
		do {} while ( CLKSYS_IsReady( OSC_RC2MRDY_bm ) == 0 );	// Internal 2MHz RC Oscillator Ready bit mask polling	
		
		if ( freq == 2 )
		{
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC2M_gc );
			CLKSYS_Disable( OSC_PLLEN_bm );
		}
		else
		{
			CLKSYS_PLL_Config( OSC_PLLSRC_RC2M_gc, (freq>>2) );
			CLKSYS_Enable( OSC_PLLEN_bm );
			CLKSYS_Prescalers_Config( CLK_PSADIV_1_gc, CLK_PSBCDIV_1_1_gc );
			do {} while ( CLKSYS_IsReady( OSC_PLLRDY_bm ) == 0 );
			CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_PLL_gc );
			
		}
		
		CLKSYS_Disable( OSC_RC32MEN_bm );					// Disable 32MHz RC Oscillator Enable bit mask		
	}
}
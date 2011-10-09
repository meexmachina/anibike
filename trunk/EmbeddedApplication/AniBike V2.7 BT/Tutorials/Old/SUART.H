#include <avr/io.h>

extern volatile uint8_t srx_done;
#define kbhit()	(srx_done)	// true if byte received

void suart_init( void );
void sputchar( uint8_t val );
uint8_t sgetchar( void );
void sputs( uint8_t *txt );


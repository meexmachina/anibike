/* Data defines
PORT
TX_MASK
PINS
RX_MASK
OCR  //TC Output Compare Register
TCNT //Acutal counter register
TCRB // TC control register b (register with clk selection options)


*/

/* Method defines
HANDEL_UART_DATA(data) //called when a data byte has sucessfully bin reseved
SETUP_TC //Setup the TC for the uart speed and TCNT = 0;
*/

/* Optional defines
FLIP_LEVELS //default rest 0, flipped to rest 1
TAKE  //Called when uart takes TC
GIVE  //Called when uart stops using TC
SENT_BYTE //Called when a data byte has bin sent
BAUD //BAUDrate 9600 baud default
CLK //clockrate default 8000000 (8MHz)
DIVIDER //Devision factor to be used default 8
TWO_STOPBITS //By default one stopbit is used by defining this two will be used
*/

// User TODOs: set interup vectors and DDRX registers, call UART_INIT() define options and funks

#ifndef BAUD
#define BAUD 9600
#endif

#ifndef CLK
#define CLK 8000000
#endif

#ifndef DIVIDER 
#define DIVIDER 8
#endif

#define TIX_PER_BIT (CLK / DIVIDER / BAUD)

#ifndef FLIP_LEVELS
#define REST_O 0
#define ACTIVE_O TX_MASK
#define REST_I 0
#define ACTIVE_I RX_MASK
#else
#define REST_O TX_MASK
#define ACTIVE_O 0
#define REST_I RX_MASK
#define ACTIVE_I 0 
#endif

uint8 flags;
uint8 nextFrame:
uint8 cntrs;
uint8 O_buffer;
uint8 I_buffer;
uint8 stack;

#define STACK_PUSH(data)         (stack = (data))
#define STACK_PEEK               (stack)
#define STACK_POP(target)        ((target) = stack)

#define NEXT_EVENT_IS_SEND_FLAG  4
#define NEXT_EVENT_IS_SEND       (flags & NEXT_EVENT_IS_SEND_FLAG)
#define SET_NEXT_EVENT_IS_SEND   (flags |= NEXT_EVENT_IS_SEND_FLAG)
#define SET_NEXT_EVENT_IS_RESEVE (flags &= ~NEXT_EVENT_IS_SEND_FLAG)

#define SENDING_FLAG             2
#define SENDING                  (flags & SENDING_FLAG)
#define START_SENDING            (flags |= SENDING_FLAG)
#define STOP_SENDING             (fags &= ~SENDING_FLAG)

#define RESEVING_FLAG            1
#define RESEVING                 (flags & RESEVING_FLAG)
#define START_RESEVING           (flags |= RESEVING_FLAG)
#define STOP_RESEVING            (flags &= ~RESEVING_FLAG)

#define UART_IS_ACTIVE           (flags & (RESEVING_FLAG | SENING_FLAG))

#define SEND_CNTR                (cntrs & 0x0F)
#define INC_SEND_CNTR            cntrs++
#define CLEAR_SEND_CNTR          (cntrs = cntrs &0xF0)

#define RES_CNTR                 (cntrs & 0xF0)
#define INC_RES_CNTR             (cntrs += 0x10)
#define CLEAR_RES_CNTR           (cntrs = cntrs & 0x0F)

#define UART_INIT()              \
	nextFrame = 0;                 \
	flags = 0;                     \
	cnters = 0;                    \
	PORT = (PORT & (~TX_MASK))

#define STOP_TC TCRB = 0;


void UART_SEND(uint8 data)
{
	if (!SENDING)
	{
		CLEAR_SEND_CNTR;
		O_buffer = data;
		nextFrame = REST_O;
		START_SENDING;
		if(!RESEVING)
		{
#ifdef TAKE
			TAKE;
#endif
			SETUP_TC;
			OCR = TIX_PER_BIT;
			SET_NEXT_EVENT_IS_SEND;
		}
		else
		{
			STACK_PUSH(TIX_PER_BIT - OCR + TCNT);
			if (STACK_PEEK < 2)
			{
				//STACK_POP();
				STACK_PUSH(2);
			}
		}
	}
}

void OnPinchange(void)
{
	if (!RESEVING && ((PINS & RX_MASK) == ACTIVE_I))
	{
		CLEAR_RES_CNTR;
		I_buffer = 0;
		START_RESEVING;
		if (!SENDING)
		{
#ifdef TAKE
			TAKE;
#endif
			SETUP_TC;
			OCR = TIX_PER_BIT / 2;
			SET_NEXT_EVENT_IS_RESEVE;
		}
		else
		{
			if ((TIX_PER_BIT / 2) > (OCR - TCNT - 2))
			{
				STACK_PUSH(TIX_PER_BIT / 2 - OCR + TCNT);
			}
			else
			{
				STACK_PUSH(OCR - TCNT - TIX_PER_BIT / 2);
				OCR = TIX_PER_BIT / 2 + TCNT;
			}
			if (STACK_PEEK <2)
			{
				//STACK_POP();
				STACK_PUSH(2);
			}
		}
	}
}

void OnOCM(void)
{
	if (NEXT_EVENT_IS_SEND)
	{
		PORT = (PORT & (~TX_MASK)) | nextFrame;

		if (RESEVING)
		{
			STACK_POP(OCR);
			SET_NEXT_EVENT_IS_RESEVE;
			STACK_PUSH(TIX_PER_BIT - OCR);
		}
		else
		{
			OCR = TIX_PER_BIT;
		}

		if (SEND_CNTR == 0)
		{// Send startbit
			nextFrame = ACTIVE_O;
		}
		else if (SEND_CNTR < 9)
		{//Send 8 bits
			nextFrame = REST_O;
			if (O_buffer & 1)
			{
				nextFrame = ACTIVE_O;
			}
			O_buffer >>= 1;
		}
#ifdef TWO_STOPBITS
		else if (SEND_CNTR < 0x0B)
#else 
		else if (SEND_CNTR < 0x0A)
#endif
		{
			nextFrame = REST_O;
		}
		else
		{
			STOP_SENDING;

			if (!RESEVING)
			{
				STOP_TC;

#ifdef GIVE
				GIVE;
#endif
			}

#ifdef SENT_BYTE
			SENT_BYTE;
#endif

		} 
		INC_SEND_CNTR;
	}
	else
	{

		if (SENDING)
		{
			STACK_POP(OCR);
			SET_NEXT_EVENT_IS_SEND;
			STACK_PUSH(TIX_PER_BIT - OCR);
		}
		else
		{
			OCR = TIX_PER_BIT;
		}

		if (RES_CNTR == 0)
		{//Read startbit
			if ((PINS & RX_MASK) != ACTIVE_I)
			{
				STOP_RESEVING;//Error

				if (!SENDING)
				{
					STOP_TC
#ifdef GIVE
						GIVE;
#endif
				}
			}
		}
		else if (RES_CNTR < 0x90)
		{//Read 8 bits
			if ((PINS & RX_MASK) == ACTIVE_I)
			{
				I_buffer += 0x80;
			}
			I_buffer >>= 1;
		}
#ifdef TWO_STOPBITS
		else if (RES_CNTR < 0xB0)
#else
		else if (RES_CNTR < 0xA0)
#endif
		{//Read stopbit
			if ((PINS & RX_MASK) != REST_I)
			{
				STOP_RESEVING;
				if (!SENDING)
				{
					STOP_TC;
#ifdef GIVE
					GIVE;
#endif
				}
			}
			else
			{
				HANDEL_UART_DATA(I_buffer);
			}
		}
		else
		{

			STOP_RESEVING;

			if (!SENDING)
			{
				STOP_TC;
#ifdef GIVE
				GIVE;
#endif
			}
		}
		INC_RES_CNTR;
	}

}
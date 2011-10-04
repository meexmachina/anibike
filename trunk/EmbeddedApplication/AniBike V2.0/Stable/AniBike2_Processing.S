#include <avr/io.h>

.extern uint8_t	*Line_Buff1;
.extern uint8_t	*Line_Buff2;
//.extern uint8_t	g_bBufferChoose;

.extern uint8_t		g_iPORTB_Buf;
.extern uint8_t		g_iPORTC_Buf;
.extern uint8_t		g_iPORTD_Buf;
.extern uint8_t		g_iPORTF_Buf;

.extern	uint8_t		g_iRowNum;
.extern	uint8_t 	g_iPWM_Counter;
.extern uint8_t		g_bHighSpeedMode;


hiSpeedMode: 
		adiw r26,48						; Jump directly to L1

		clr r16
		sts g_iPORTC_Buf, r16			; Store Port C = 0
		jmp L1_starts_here				; jump to L1 stuff


.global PrepareBuffers
PrepareBuffers:
		push r1
		push r0
		in r0, 0x3f			// Store the status register
		push r0
		clr r1 					// Clear register

		push XH
		push XL
		push r16
		push r18
		push r19
		push r20


		in r18, 0x1e					; Load the row number from GPIOR0
		andi r18, 0b00000111
		lsl r18							; offset = row_number*2
		in	r19, 0x2a					; Load PWM counter for comparison from GPIOR1


		in r20, 0x1e					; check which buffer is used
		andi r20, 0b01000000
//		tst r20
		breq second_buffer

		ldi XL, lo8(Line_Buff1)	; Put the Array start to fill 
		ldi XH, hi8(Line_Buff1)
		brne buffer_choose_end
second_buffer:
		ldi XL, lo8(Line_Buff2)	; Put the Array start to fill 
		ldi XH, hi8(Line_Buff2)

buffer_choose_end:

		add XL, r18 					; Go to the offset position
		adc XH, r1						; r1 = zero
		in r16, 0x1e					; are we in the hispeed mode
		andi r16, 0b10000000		/////////////////////////////////////////////////////////////////////////////////////////////
		//cpi r16, 1						; if ==1 - in high speed mode
		brne hiSpeedMode

		; ===========================================================================
		;	PORT C BUFFER
		; ===========================================================================
		
		//clr r16							; clear the register which contains buffer
		; Start loading for Blue L2		<==========================================
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
			
		adiw r26,14						; Next Color
		; Start loading for Green L2	<==========================================
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1

		sts g_iPORTC_Buf, r16			; Store Port C



		; ===========================================================================
		;	PORT F BUFFER
		; ===========================================================================
		adiw r26,14						; Next Color
		clr r16							; Clear the register for new port
		; Start loading for RED L2		<==========================================
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16

		adiw r26,14						; Next Color
		; Start loading for BLUE L1		<==========================================
L1_starts_here:
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1

		sts g_iPORTF_Buf, r16			; Store Port F



		; ===========================================================================
		;	PORT D BUFFER
		; ===========================================================================

		adiw r26,14						; Next Color
		; Start loading for GREEN L1	<==========================================
		clr r16							; Start over

		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		lsl r16
		sts g_iPORTD_Buf, r16			; Store Port D



		; ===========================================================================
		;	PORT B BUFFER
		; ===========================================================================

		adiw r26,14						; Next Color
		; Start loading for RED L1		<==========================================
		clr r16
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #0
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #1
		cp	r19, r20					; compare to influence Carry
		adc r16, r1
		lsl r16
		
		ld r20, X+						; Read the location
		mov r18, r20					; r18 contains #2
		swap r18						; swap nibbles
		andi r18, 0xF					; only %0 remains
		cp  r19, r18					; compare to influence Carry
		adc r16, r1						; add zero+Carry to result
		lsl r16
		
		andi r20, 0xF					; get #3
		cp	r19, r20					; compare to influence Carry
		adc r16, r1

		swap r16						; Switch nibbles
		sts g_iPORTB_Buf, r16			; Store Port B


		pop r20
		pop r19
		pop r18
		pop r16
		pop XL
		pop XH

		pop r0
		out 0x3F,r0
		pop r0
		pop r1

		ret 				// Return to mother function


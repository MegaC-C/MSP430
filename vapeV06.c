//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.4
//
//  Description; Toggle P1.4 by xor'ing P1.4 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430i20xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.4|-->LED
//
//  Texas Instruments, Inc
//  July 2011
//***************************************************************************************

#include <msp430.h>



const char red = 'r';
const char yellow = 'y';
const char green = 'g';
const char turquoise = 't';
const char blue = 'b';
const char purple = 'p';
const char white = 'w';
const char off = 'o';

void LED(char color){
    P2OUT &= ~BIT0 & ~BIT5 & ~BIT1;
    switch(color){
        case 'r':
            P2OUT |= BIT0;
            break;
        case 'y':
            P2OUT |= BIT0  | BIT5;
            break;
        case 'g':
            P2OUT |= BIT5;
            break;
        case 't':
            P2OUT |= BIT5 | BIT1;
            break;
        case 'b':
            P2OUT |= BIT1;
            break;
        case 'p':
            P2OUT |= BIT0  | BIT1;
            break;
        case 'w':
            P2OUT |= BIT0  | BIT5 | BIT1;
            break;
        case 'o':
            break;
        default:
            P2OUT |= BIT0  | BIT5 | BIT1;
            break;
    }
}

void main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	P2DIR |= BIT0;					// Set P1.4 to output direction
	P2DIR |= BIT1;
	P2DIR |= BIT5;


	long i = 500000;

	while(1){

	    if(P1IN & BIT1){
	              while(i != 0)i--;
	              LED(red);
	              i = 500000;
                  while(i != 0)i--;
                  LED(yellow);
                  i = 500000;
                  while(i != 0)i--;
                  LED(green);
                  i = 500000;
                  while(i != 0)i--;
                  LED('t');
                  i = 500000;
                  while(i != 0)i--;
                  LED(blue);
                  i = 500000;
                  while(i != 0)i--;
                  LED(purple);
                  i = 500000;
                  while(i != 0)i--;
                  LED(white);
                  i = 500000;
                  while(i != 0)i--;
                  LED(off);
                  i = 500000;




	    }
	    else P2OUT &= ~BIT0 & ~BIT5 & ~BIT1;

//	    while(i != 0)i--;
//	    P2OUT ^= BIT0;
//	    P2OUT ^= BIT1;
//	    i = 500000;
//	    while(i != 0)i--;
//	            P2OUT ^= BIT1;
//	            P2OUT ^= BIT5;
//	            i = 500000;
//	            while(i != 0)i--;
//	                    P2OUT ^= BIT5;
//	                    P2OUT ^= BIT0;
//	                    i = 500000;


	}

}

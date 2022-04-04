#include <msp430.h> 

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// -- configure Port --
	P2DIR |= BIT5;              // blueLED

	// -- configure Timer A0 --
	TA0CTL |= TACLR;            // clear timer settings
	TA0CTL |= TASSEL_1;         // ACLK as source
	TA0CTL |= MC_1;             // up mode
	TA0CCR0 = 100;
	TA0CCR1 = 99;               // 1% duty cycle

	// -- enable Timer compare interrupts CCR0 and CCR1
	TA0CCTL0 |= CCIE;           // enable local interrupt
	TA0CCTL0 &= ~CCIFG;         // clear flag
    TA0CCTL1 |= CCIE;           // enable local interrupt
    TA0CCTL1 &= ~CCIFG;         // clear flag
	__enable_interrupt();       // enable global interrupts

	while(1){}                  // loop

	return 0;
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void off(void){         // LED off
    P2OUT &= ~BIT5;
    TA0CCTL0 &= ~CCIFG;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void on(void){          // LED on
    P2OUT |= BIT5;
    TA0CCTL1 &= ~CCIFG;
}

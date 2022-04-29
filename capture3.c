#include <msp430.h>

#define debounceTIME 1600  	// 50ms @ ACLK; 1 = 31,25ms
#define onTIME 150		// 300s @ ACLK; 1 = 2s

unsigned int lastTime, capturedTime, deltaTime;
int pressButton = 3;
int onCounter = onTIME

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    // ---setup LED---
    P2DIR |= BIT0 | BIT5 | BIT1; //Set LED rgb as output
    
    // ---setup hall switch---
    P1DIR &= ~BIT1;     // set as input
    P1IES |= BIT1;		// interrupt on high-to-low transition, hall sensor high = off
    P1IFG &= ~BIT1;	// clear IFG flag
    P1IE |= BIT1;		// enable local interrupt

    // ---setup Timer---  as described in MSP430 workshop series
    // 1.configure Timer
    TA1CTL |= TACLR;        // clear TA1
    TA1CTL |= TASSEL_1 | TAIE;  // ACLK as source, enable TA1IFG interrupt
    // 2.configure CCRx
    TA1CCTL1 |= CAP | CCIS_2 | CM_3; // CCR1 in capture mode, select GND as initial input signal, sensitive to switching to VCC and GND
    // 3.clear IFG and start timer
    // TA1CTL &= ~TAIFG;
    // TA1CTL |= MC_1;

    __enable_interrupt();   // enable global interrupts

    while (1)
    {
    	if(P1IN & BIT1)
    	{
    		switch(pressButton)
    		{
    			case 3:
    			LED(off);
    			break;
    			case 2:
    			LED(blue);
    			break;
    			case 1:
    			LED(purple);
    			break;
    			case 0:
    			LED(red);
    			break;
    			default:
    			brea
    			
    				
    	}
        if (pressButton == 0 && (P1IN & BIT1))
        {
            P2OUT = BIT0;   // red on
        }
        else
        {
            P2OUT &= ~BIT0;   // red off
        }
    }
    return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void myISR_Port1(void)
{
    TA1CCTL1 ^= CCIS0;      // toggle VCC and GND to capture TA1.1
    lastTime = capturedTime;
    capturedTime = TA1CCR1;
    deltaTime = capturedTime - lastTime;
    if(deltaTime > debounceTIME && pressButton != 0)
    {
    	--pressButton;
    }
    TA1CTL |= TACLR;		// clear TA1R = 0, stop TA1
    TA1CTL &= ~TAIFG;	// clear TA1 IFG
    TA1CTL |= MC_1;		// start TA1
    
    onCounter = onTIME;
    
     P1IFG &= ~BIT1;
}

#pragma vector = TIMER1_A1_VECTOR
__interrupt void myISR_TA1_other(void)
{
    if(pressButton == 0)
    {
    	--onCounter;		// turn device off after given time
    }
    else
    {
    	pressButton = 3;	// reset button input
    }
    
    TA1CTL &= TAIFG;
}

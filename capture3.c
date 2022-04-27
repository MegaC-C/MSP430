#include <msp430.h> 

unsigned int lastTime, capturedTime, deltaTime;
int pressButton = 3;
bool active = 1;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // stop watchdog timer

    //---setup Timer---
    //1.configure Timer
    TA0CTL |= TACLR;               //clear TA0
    TA0CTL |= TASSEL_1 | TAIE;     //ACLK as source, enable TA0IFG interrupt
    //2.configure CCRx
    TA0CCTL1 |= CAP | CM__BOTH | CCIS__GND;		//CCR1 in capture mode, sensitive to switching to VCC and GND, select GND as initial input signal
    //3.clear IFG and start timer
    TA0CTL &= ~TAIFG;
    TA0CTL |= MC_1;

    __enable_interrupt();                    // enable global interrupts

    while (1)
    {
        if(active == 1 && 
        
        else
        {
        	pressButton = 3;
        }  
    }
    return 0;
}

#pragma vector = PORT1_VECTOR
__interrupt void myISR_Port1(void){
	TA0CCTL1 ^= CCIS0;				// toggle VCC and GND to capture TA0.1
	lastTime = capturedTime;
	capturedTime = TA0CCR1;
	deltaTime = capturedTime - lastTime;
	if(minTime < deltaTime)
	{
		if(deltaTime < maxTime)
		{
			--pressButton;
		}
		else
		{
			pressButton = 2;
		}
	}
	P1IFG &= ~BIT1;
}

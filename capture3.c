#include <msp430.h>

#define minTime 1
#define maxTime 1

unsigned int lastTime, capturedTime, deltaTime;
int pressButton = 2;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer

    P2DIR |= BIT0 | BIT5 | BIT1; //Set LED rgb as output
    P1DIR &= ~BIT1;     // set as input

    // ---setup Timer---
    // 1.configure Timer
    TA0CTL |= TACLR;        // clear TA0
    TA0CTL |= TASSEL_1 | TAIE;  // ACLK as source, enable TA0IFG interrupt
    // 2.configure CCRx
    TA0CCTL1 |= CAP | CCIS_2 | CM_3; // CCR1 in capture mode, select GND as initial input signal, sensitive to switching to VCC and GND
    // 3.clear IFG and start timer
    TA0CTL &= ~TAIFG;
    TA0CTL |= MC_1;

    __enable_interrupt();   // enable global interrupts

    while (1)
    {
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
    TA0CCTL1 ^= CCIS0;      // toggle VCC and GND to capture TA0.1
    lastTime = capturedTime;
    capturedTime = TA0CCR1;
    deltaTime = capturedTime - lastTime;
    if (minTime < deltaTime && pressButton != 0)
    {
        if (deltaTime < maxTime)
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

#pragma vector = TIMER0_A1_VECTOR
__interrupt void myISR_TA0_other(void)
{
    if(pressButton == 0)
    {

    }
    TA0CTL &= TAIFG;
}

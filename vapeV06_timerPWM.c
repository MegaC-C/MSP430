#include <msp430.h> 

//to do:
//beschriften

#define myCOUNTNUMBER 10                // "define" because "const int" is not C compatible
unsigned int raw;
int counter = myCOUNTNUMBER;		// defines sample frequency (64Hz/10=6Hz)

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // stop watchdog timer

    P1SEL1 |= BIT5;		// Pin in CCRx mode for PWM (mosfet), must be internally connected to TimerA
    P1DIR |= BIT5;		// Pin as output, if PxSELx = 0.0 (back to standard)
    P1OUT |= BIT5;		//Pin HIGH when in output mode

    P1DIR |= BIT4;		// Pin for time measurements
    
    SD24CTL = SD24REFS;                        // Internal ref
    SD24CCTL0 |= SD24SNGL | SD24DF | SD24IE; // single conversion, 2s compliment
    SD24INCTL0 |= SD24INCH_6;                  // Internal temp sensor
    __delay_cycles(3200);                 // Delay ~200us for 1.2V ref to settle

    //---setup Timer---
    //1.configure Timer
    TA0CTL |= TACLR;               //clear TA0
    TA0CTL |= TASSEL_1 | TAIE;     //ACLK as source, enable TA0IFG interrupt
    //2.configure CCRx
    TA0CCTL0 |= CCIE;              //enable TA0CCR0 interrupt
    TA0CCR0 = 512;                 //results in ~64Hz frequency
    TA0CCTL1 |= OUTMOD_7;          //Reset/Set mode
    TA0CCR1 = 0;                   //start with 0=0% duty cycle, 128=25%, 256=50% etc.
    //3.clear IFG and start timer
    TA0CCTL0 &= ~CCIFG;
    TA0CTL &= ~TAIFG;
    TA0CTL |= MC_1;

    __enable_interrupt();                    // enable global interrupts

    while (1)
    {
        __delay_cycles(64000);
    }
    return 0;
}

#pragma vector=TIMER0_A0_VECTOR                         // fires when timer counts to CCR0
__interrupt void myISR_TA0_CCR0(void)
{
    --counter; 		// used to lower sample freqeuncy
    if (counter == 0)
    {
        P1SEL1 &= ~BIT5;                    //Pin back to normal mode
        P1OUT ^= BIT4;		// for time measurement
    }
    TA0CCTL0 &= ~CCIFG;
}
#pragma vector=TIMER0_A1_VECTOR                         // fires when timer counts to 0; ne TA0 cycle between CCR0 and TA0IV ~30us to fully saturate mosfet gate capacitance, >10 times excess (25nC/10mA=2.5us)
__interrupt void myISR_TA0_other(void)
{
    if (counter == 0)
    {
        P1OUT ^= BIT4;		// for time measurement
        SD24CCTL0 |= SD24SC;                // Set bit to start SDADC conversion
        P1OUT ^= BIT4;		// for time measurement
        counter = myCOUNTNUMBER;		// reset counter
    }
    TA0CTL &= ~TAIFG;
}
#pragma vector=SD24_VECTOR
__interrupt void myISR_SD24(void)
{
    raw = SD24MEM0;                    // Save SD24 results (clears IFG)
    P1SEL1 |= BIT5;		// Pin back in CCRx mode
    P1OUT ^= BIT4;		// for time measurement
    SD24CCTL0 &= ~SD24IFG;
}






























#include <msp430.h> 

//to do:
//timer um SDADC nur ca 10mal pro sekunde zu starten
//beschriften

unsigned int raw;
enum {countNumber=20};          // "enum" because "const int" is not C compatible
int counter=countNumber;

int main(void){
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	P1SEL1 |= BIT5;
	P1DIR |= BIT5;
	P1OUT |= BIT5;
	
	//---setup Timer---
	//1.configure Timer
    TA0CTL |= TACLR;
    TA0CTL |= TASSEL_1 | TAIE;
	//2.configure CCRx
	TA0CCTL0 |= CCIE;
	TA0CCR0 = 400;
	TA0CCTL1 |= OUTMOD_7;
	TA0CCR1 = 100;
	//3.clear IFG and start timer
	TA0CCTL0 &= ~CCIFG;
	TA0CTL &= ~TAIFG;
	TA0CTL |= MC_1;

    SD24CTL = SD24REFS;                         // Internal ref
    SD24CCTL0  |= SD24SNGL | SD24DF | SD24IE;   // single conversion, 2s compliment
    SD24INCTL0 |= SD24INCH_6;                   // Internal temp sensor

    __delay_cycles(3200);                       // Delay ~200us for 1.2V ref to settle

    __enable_interrupt();                       // enable global interrupts

    while(1) {
        __delay_cycles(64000);
    }
    return 0;
}

#pragma vector=SD24_VECTOR
__interrupt void myISR_SD24(void){
    raw = SD24MEM0;                             // Save SD24 results (clears IFG)
    P1SEL1 |= BIT5;
    //TA0CTL |= MC_1;
    SD24CCTL0 &= ~SD24IFG;
}
#pragma vector=TIMER0_A0_VECTOR                 // IFG cleared automatically
__interrupt void myISR_TA0_CCR0(void){
    --counter;
    TA0CCTL0 &= ~CCIFG;
}
#pragma vector=TIMER0_A1_VECTOR 				 // one TA0 cycle between CCR0 and TA0IV ~30us to fully saturate mosfet gate capacitance, 10 times excess (25nC/10mA=2.5us)
__interrupt void myISR_TA0_other(void){
	if(counter==0){
		//TA0CTL |= MC_1;
        P1SEL1 &= ~BIT5;       
        SD24CCTL0 |= SD24SC;                    // Set bit to start conversion
        counter=countNumber;
    }
    TA0CTL &= ~TAIFG;
}
































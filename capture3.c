#pragma vector = PORT1_VECTOR
__interrupt void myISR_Port1(void){
	TA1CCTL1 ^= CCIS0;				// toggle VCC and GND to capture TA1.1
	capturedTime = TA1CCR1;
	if(minTime < capturedTime && capturedTime < maxTime){
		++pressButton;
	}
	P1IFG &= ~BIT1;
}

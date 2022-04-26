unsigned int lastTime, capturedTime, deltaTime;
int pressButton;
bool active = 1;

#pragma vector = PORT1_VECTOR
__interrupt void myISR_Port1(void){
	TA1CCTL1 ^= CCIS0;				// toggle VCC and GND to capture TA1.1
	lastTime = capturedTime;
	capturedTime = TA1CCR1;
	deltaTime = capturedTime - lastTime;
	if(minTime < deltaTime){
		if(deltaTime < maxTime){
			--pressButton;
		}
		else{
			pressButton = 2;
		}
	}
	P1IFG &= ~BIT1;
}

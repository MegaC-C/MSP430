

int main (){
	
	// -- LED output --
	P2DIR |= BIT4;
	
	// -- switch input --
	P2DIR &= ~BIT3;
	P2IES |= BIT3;
	P2IE |= BIT3;
	P2IFG &= ~BIT3;
	
	// -- uart --
	UC0CTLW0 |= UCSWRST;
	UC0CTLW0 |= UCSSEL__SMCLK;
	UCA0BRW = 8;
	
	P1SEL1 &= ~BIT3;
	P1SEL0 |= BIT3;
	
	UCA0MCTLW |= F7E1;
		
	__enable_interrupt();
	
	while(1){};
	
	#pragma vector = ;
	__interrupt void toggleTX(void){
		P2OUT ^= BIT4;
		UCA0TXBUF = 0b10101010;
		P2IFG &= ~BIT3;
	}
	
	return 0;
}
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
		case 'o':
			break;
		default:
			P2OUT |= BIT0  | BIT5 | BIT1;
	}
}
#include <msp430.h> 

int _system_pre_init(void)
{
    unsigned long *jtagPwd = (unsigned long *)JTAG_DIS_PWD1;

    /* Feed the watchdog timer */
    WDTCTL = WDTPW | WDTCNTCL;

    /* Check JTAG password locations and disable JTAG if passwords don't match.
     * Else the JTAG will be enabled in the 64th cycle after reset.
     */
    if ((*jtagPwd != 0x00000000) && (*jtagPwd != 0xFFFFFFFF))
    {
        /* Disable JTAG */
        SYSJTAGDIS = JTAGDISKEY;
    }

    /* Calibration section
     * Check for the BORIFG flag in IFG1. Execute calibration if this was a BORIFG.
     * Else skip calibration
     */
    if (IFG1 & BORIFG)
    {
        /* Perform 2's complement checksum on 62 bytes of TLV data */
        unsigned int checksum = 0;
        unsigned char *TLV_address_for_parse = ((unsigned char *)TLV_START);
        unsigned int *TLV_address_for_checksum = ((unsigned int *)TLV_START + 1);

        do
        {
            checksum ^= *TLV_address_for_checksum++;
        } while (TLV_address_for_checksum <= (unsigned int *)TLV_END);

        checksum ^= 0xFFFF;
        checksum++;

        /* If check sum is not correct go to LPM4 */
        if (*((unsigned int *)TLV_START) != checksum)
        {
            /* Enter LPM4 if checksum failed */
            __bis_SR_register(LPM4_bits);
        }

        /* Check sum matched, now set calibration values */

        /* Calibrate REF */
        REFCAL1 = *(TLV_address_for_parse + TLV_CAL_REFCAL1);
        REFCAL0 = *(TLV_address_for_parse + TLV_CAL_REFCAL0);

        /* Calibrate DCO */
        CSIRFCAL = *(TLV_address_for_parse + TLV_CAL_CSIRFCAL);
        CSIRTCAL = *(TLV_address_for_parse + TLV_CAL_CSIRTCAL);
        CSERFCAL = *(TLV_address_for_parse + TLV_CAL_CSERFCAL);
        CSERTCAL = *(TLV_address_for_parse + TLV_CAL_CSERTCAL);

        /* Calibrate SD24 */
        SD24TRIM = *(TLV_address_for_parse + TLV_CAL_SD24TRIM);

        /* Clear BORIFG */
        IFG1 &= ~(BORIFG);
    }

    /* Feed the watchdog timer */
    WDTCTL = WDTPW | WDTCNTCL;

    /* Return value:
     *  1 - Perform data segment initialization.
     *  0 - Skip data segment initialization.
     */
    return 1;
}

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

    TA0CTL |= TACLR;
	TA0CCR0 = 400;
	TA0CCR1 = 100;
	TA0CCTL0 |= CCIE;
	TA0CCTL1 |= OUTMOD_7;
	TA0CTL |= TASSEL_1;
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
__interrupt void ISR_SD24(void){
    raw = SD24MEM0;                             // Save SD24 results (clears IFG)
    P1SEL1 |= BIT5;
    TA0CTL |= MC_1;
}
#pragma vector=TIMER0_A0_VECTOR                 // IFG cleared automatically
__interrupt void ISR_TimerA0CCR0(void){
    --counter;
    if(counter==0){
        TA0CTL &= ~MC_1;
        P1SEL1 &= ~BIT5;
        __delay_cycles(400);                    // wait ~25us to fully saturate mosfet gate capacitance, 10 times excess (25nC/10mA=2.5us)
        SD24CCTL0 |= SD24SC;                    // Set bit to start conversion
        counter=countNumber;
    }
}









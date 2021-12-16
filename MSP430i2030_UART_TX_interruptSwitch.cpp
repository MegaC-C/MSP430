#include <msp430.h> 

#ifdef __TI_COMPILER_VERSION__
int _system_pre_init(void)
#elif __IAR_SYSTEMS_ICC__
int __low_level_init(void)
#elif __GNUC__
extern int system_pre_init(void) __attribute__((constructor));
int system_pre_init(void)
#else
#error Compiler not supported!
#endif
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


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// -- LED output --
	    P2DIR |= BIT4;

	    // -- switch input --
	    P2DIR &= ~BIT3;
	    P2IES |= BIT3;
	    P2IFG &= ~BIT3;
	    P2IE |= BIT3;


	    // -- uart --
	    UCA0CTLW0 |= UCSWRST;
	    UCA0CTLW0 |= UCSSEL__SMCLK;
	    UCA0BRW = 8;
	    UCA0MCTLW |= 0xF7E1;

	    P1SEL1 &= ~BIT3;
	    P1SEL0 |= BIT3;

	    UCA0CTLW0 &= ~UCSWRST;

	    __enable_interrupt();

	    while(1){};



	return 0;
}

#pragma vector = PORT2_VECTOR;
__interrupt void toggleTX(void){
    P2OUT ^= BIT4;
    UCA0TXBUF = 'A';
    P2IFG &= ~BIT3;
    }


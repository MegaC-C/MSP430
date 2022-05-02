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

/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430i20xx Demo - SD24, Continuous Conversion on a Group of 3 Channels
//
//  Description: This program uses the SD24 module to perform continuous
//  conversions on a group of channels (0, 1 and 2). A SD24 interrupt occurs
//  whenever the conversions have completed.
//
//  Test by applying voltages to the 3 input channels and setting a breakpoint
//  at the indicated line. Run program until it reaches the breakpoint, then use
//  the debugger's watch window to view the conversion results.
//
//  Results (upper 16 bits only) are stored in three arrays, one for each channel.
//
//  ACLK = 32kHz, MCLK = SMCLK = Calibrated DCO = 16.384MHz, SD_CLK = 1.024MHz
//  * Ensure low_level_init.c is included when building/running this example *
//
//  Notes: For minimum Vcc required for SD24 module - see datasheet
//         100nF cap btw Vref and AVss is recommended when using 1.2V ref
//
//               MSP430i20xx
//             -----------------
//         /|\|                |
//          | |                |
//          --|RST             |
//            |                |
//   Vin1+ -->|A0.0+      VREF |---+
//   Vin1- -->|A0.0-           |   |
//   Vin2+ -->|A1.0+           |  -+- 100nF
//   Vin2- -->|A1.0-           |  -+-
//   Vin3+ -->|A2.0+           |   |
//   Vin3- -->|A2.0-      AVss |---+
//
//  T. Witt
//  Texas Instruments, Inc
//  September 2013
//  Built with Code Composer Studio v5.5
//******************************************************************************
#include "msp430.h"

unsigned int rawBattery, rawHeater, rawTemp;        // raw Bit values for Battery 0.0, Heater 1.0 and Temp 2.0
double battery_V, heater_max, temp_C;
const double Vref = 1.158;

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;                      // Stop WDT

    P1DIR |= BIT6;                                  // P1.6 Mosfet as Output
    P2DIR |= BIT4;                                  // LED 2.4 as Output

    SD24CTL = SD24REFS;                            // Internal ref
    SD24CCTL0  |= SD24GRP | SD24DF;                // Group with CH1
    SD24CCTL1  |= SD24GRP | SD24DF;                // Group with CH2
    SD24CCTL2  |= SD24IE | SD24DF;                 // Enable interrupt
    SD24INCTL1 |= SD24GAIN_16;                      // gain x16 at mosfetShunt
    SD24INCTL2 |= SD24INCH_6;                       // internal TempSensor

    __delay_cycles(3200);                          // Delay ~200us for 1.2V ref to settle

    __enable_interrupt();


    while (1){

        if (!(P1IN & BIT3)) {
            SD24CCTL2 |= SD24SC;                       // Set bit to start conversion
            P2OUT |= BIT4;                              // LED 2.4 on
            if (heater_max < 2.2) P1OUT |= BIT6;        // mosfet on
            else P1OUT &= ~BIT6;                        // mosfet off


        }
        else {
            SD24CCTL2 &= ~SD24SC;                         // clear bit to stop conversion
            P2OUT &= ~BIT4;                                 // LED 2.4 off
            P1OUT &= ~BIT6;                              // mosfet off
        }

        battery_V = (Vref*rawBattery)/6230;
        temp_C = ((rawTemp * 1200.0)/70711) - 273;

        heater_max = (double)rawBattery/rawHeater;            // @RT and rawBatter=22000 (battery_V=3.9V) => rawHeater=19000

        __no_operation();
        __no_operation();
        __no_operation();
    }
}


#pragma vector=SD24_VECTOR
__interrupt void SD24_ISR(void)
{
    rawBattery = SD24MEM0;
    rawHeater = SD24MEM1;
    rawTemp = SD24MEM2;
}


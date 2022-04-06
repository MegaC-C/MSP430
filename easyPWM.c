  ​#​include​ ​"​msp430.h​" 
  
 ​int ​main​(​void​){
 ​    WDTCTL = WDTPW | WDTHOLD;           ​//​ Stop watchdog timer 
  
 ​    P1SEL1 |= BIT5;                     ​//​ P1.5 CCRx Function 
 ​    P1DIR  |= BIT5;                     ​//​ Set P1.5 as output 
  
 ​    TA0CCR0 = ​100​; 
 ​    TA0CCTL1 |= OUTMOD_6;                ​//​ CCR1 Toggle/set 
 ​    TA0CCR1 = ​50; 
 ​   TA0CTL = TACLR;
 TA0CTL |= TASSEL_1 | MC_1;    ​//​ ACLK, Up Mode
 
     return 0;
 }
 ​
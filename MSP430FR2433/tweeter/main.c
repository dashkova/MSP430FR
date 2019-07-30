
#include "io430.h"
#include <stdint.h>


#define FRQ_TWEET   2
#define PAUSE_TWEET 400
#define CNT_TWEET   10

#define MEANDR      127



    //.. Global variation
volatile uint8_t    cnt_tweet;
//volatile uint8_t    frq_tweet;
//volatile uint8_t    PAUSE_TWEET;

enum    state_enum
        {
        pwm,
        pause,
        sleep
        } volatile  state = pwm;


inline void Set_State(void)
{
    switch (state)
    {
    case pwm:
     //   frq_tweet = 0;

        TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;

       // RTCMOD = FRQ_TWEET;

        state = pause;
        break;

    case pause:
        //cnt_tweet++;

        TA0CTL = MC__STOP;

        if (--cnt_tweet)
        {
            state = pwm;
        }
        else
            {
                state = sleep;

                RTCMOD = PAUSE_TWEET;
            }        
        break;

    case sleep:
        cnt_tweet = CNT_TWEET;

        RTCMOD = FRQ_TWEET;

      //  TA0CTL = MC__STOP;

        state = pwm;

        break;

    }
}

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                    // Stop WDT

    // Configure GPIO
    P1DIR = 0xFF;// BIT1 | BIT2;
    P2DIR = 0xFF;
   // P1OUT |= BIT1;
    P1SEL1 = BIT1 | BIT2;


    PM5CTL0 &= ~LOCKLPM5;

    // Configure Timer_A
    TA0CCR0 = 256;

    TA0CCTL1 = OUTMOD_2;                                                // Output mode Capture/Compare1 toggle/reset
    TA0CCR1 = MEANDR;

    TA0CCTL2 = OUTMOD_6;                                                // Output mode Capture/Compare2 toggle/set
    TA0CCR2 = MEANDR;

    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;// | TAIE;          

    // Configure RTC
    RTCMOD = FRQ_TWEET;

    RTCCTL = RTCSS__VLOCLK | RTCPS__100 | RTCIE | RTCSR;                // RTC period ~0.1s, enable interrupt

    PMMCTL0 = PMMPW;// | PMMREGOFF;

    __bis_SR_register(LPM4_bits | GIE);                                 // Enter LPM4, enable interrupts
   // __no_operation();                                               // For debugger
}

#pragma vector=RTC_VECTOR
    __interrupt void RTC_ISR(void)
{
    switch (RTCIV)
    {
    case RTCIV_NONE:
        break;

    case RTCIV_RTCIF:
        Set_State();
        break;     

    }
}

/*
#pragma vector=TIMER1_A0_VECTOR
    __interrupt void TIMER1_A0_ISR(void)
{
    switch(__even_in_range(TA0IV, TA0IV_TAIFG))
    {
        case TA0IV_NONE:
            break;                               // No interrupt
        case TA0IV_TACCR1:
            break;                               // CCR1 not used
        case TA0IV_TACCR2:
            break;                               // CCR2 not used
        case TA0IV_TAIFG:

            break;
        default:
            break;
    }
}       */
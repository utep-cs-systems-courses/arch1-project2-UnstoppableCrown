#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

#define LED_RED BIT0 // P1.0
#define LED_GREEN BIT6 //P1.6
#define LEDS (LED_RED | LED_GREEN)

#define SW1 BIT3 // switch1 is p1.3
#define SWITCHES SW1

void switch_init()
{
  P1REN |= SWITCHES;
  P1IE |= SWITCHES;
  P1OUT |= SWITCHES;
  P1DIR &= ~SWITCHES;
}

void led_init()
{
  P1DIR |= LEDS;
  P1OUT &= ~LEDS;
}
void wdt_init()
{
  configureClocks;
  enableWDTInterrupts();
}

int main() {

  switch_init();
  buzzer_init();
  led_init();
  wdt_init();
  // buzzer_set_period(1000);/* start buzzing!!! 2MHz/1000 = 2kHz*/


  or_sr(0x18);          // CPU off, GIE on
}

static int buttonDown;

void switch_interrupt_handler()
{
  char p1val = P1IN;

  P1IES |= (p1val & SWITCHES);
  P1IES &= (p1val | ~SWITCHES);

  if (p1val & SW1)
    {
      P1OUT &= ~LED_GREEN;
      buzzer_set_period(0);
      buttonDown = 0;
    }
  else
    {
      P1OUT |= LED_GREEN;
      buzzer_set_period(1000);
      buttonDown = 1;
    }
}

void __interrupt_vec(PORT1_VECTOR) Port_1()
{
  if (P1IFG & SWITCHES)
    {
      P1IFG &= ~SWITCHES;
      switch_interrupt_handler();
    }
}

void __interrupt_vec(WDT_VECTOR) WDT()
{
  static int blink_count = 0;
  switch (blink_count)
    {
    case 6:
      blink_count = 0;
      P1OUT |= LED_RED;
      break;
    default:
      blink_count ++;
      if (!buttonDown) P1OUT &= ~LED_RED;
    }
}

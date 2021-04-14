#include <Arduino.h>
#include <sapi_delay.h>

/* ---- Non Blocking Delay ---- */

void delayInit( delay_t * delay, tick_t duration )
{
   delay->duration = duration;
   delay->running = 0;
}

bool_t delayRead( delay_t * delay )
{
   bool_t timeArrived = 0;

   if( !delay->running ) {
      delay->startTime = millis();
      delay->running = 1;
   } else {
      if ( (tick_t)(millis() - delay->startTime) >= delay->duration ) {
         timeArrived = 1;
         delay->running = 0;
      }
   }

   return timeArrived;
}

void delayWrite( delay_t * delay, tick_t duration )
{
   delay->duration = duration;
}


#include <sapi_datatypes.h>

typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;


/* ---- Non Blocking Delay ---- */
void delayInit( delay_t * delay, tick_t duration );
bool_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, tick_t duration );

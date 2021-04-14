#include <sapi_datatypes.h>
/*
typedef struct{
    uint16_t velocidad;
    uint16_t rumbo;
} gpsDatos_t;
*/

String getDataGPS();
bool getDataSerial(Stream &puertoSerie);
void cleanBuff();
void processCommand();

bool calcCHK(char *s);
uint8_t convertCharToInt(char data);

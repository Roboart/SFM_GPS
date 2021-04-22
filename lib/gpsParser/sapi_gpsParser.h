#include <sapi_datatypes.h>

typedef enum{
    NoFix = false,
    Fix3D = true
} validez;

typedef struct{
    uint8_t dia;
    uint8_t mes;
    uint8_t anio;
    uint8_t hora;
    uint8_t minuto;
    uint8_t segundo;
    float latitud;
    float longitud;
    uint16_t velocidad;
    uint16_t rumbo;
    bool isProcessed;
    bool isNewData;
    validez isValid;
} gpsDatos_t;


void gpsDatosInit(gpsDatos_t *datosGPS); /* Inicializa los datos de la coordenada*/
bool gpsNewData(Stream &puertoSerie); /* construye la cadena y calcula el checksum del paquete recibido retorna un bool*/
String getDataGPS(); /* Devuelve como String la cadena recibida */
void gpsFillData(gpsDatos_t *datosGPS);/* Identifica el tipo de paquete de datos recibidos y llama a la funcion que lo parsea*/
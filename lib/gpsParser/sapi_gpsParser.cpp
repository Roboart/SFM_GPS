#include <sapi_gpsParser.h>


const uint16_t lenbuff = 500;
static char bufferSerial[lenbuff];
int xbuff = 0;
bool flagCommand = false;


//** PROTOTIPO DE FUNCIONES PRIVADAS **//

static void cleanBuff(); // Borra Buffer.
static bool calcCHK(char *s); //Calcula el Checksum del dato para validar que viene correcto.
static uint8_t convertCharToInt(char data); //convierte de Char a int.
static void parseRMC(gpsDatos_t *datosGPS); //Parsea los datos de posicion RMC.


//** FUNCIONES PUBLICAS **//

bool gpsNewData(Stream &puertoSerie){
    char charData;
    if(puertoSerie.available()){
        charData = char(puertoSerie.read());
        switch (charData){
            case '\r':  break;
            case '\n':  if(calcCHK(&bufferSerial[0])){ //valida el checksum de la cadena
                            return true; // y si es valida permite que se procese la cadena.
                        }
                        else{
                            cleanBuff(); //si el CHK es erroneo, borra el buffer.
                        }
                        break;
            //case '$':   newData = true;
            default:    bufferSerial[xbuff++] = charData;   //en caso que se reciban mas datos 
                        if(xbuff >= lenbuff){               // que el largo de la cadena
                            xbuff = 0;                      // la vuelve a inicializar
                        }                                   // para que de error de CHK y borre el buffer.
                        break;
        }
    }
    return false;
}

String getDataGPS(){
    if(calcCHK(&bufferSerial[0])){
        return bufferSerial;
    }
    return "ErrCalcCHK";
}

void gpsDatosInit(gpsDatos_t *datosGPS){
    datosGPS->anio = 0;
    datosGPS->mes = 1;
    datosGPS->dia = 1;
    
    datosGPS->hora = 0;
    datosGPS->minuto = 0;
    datosGPS->segundo = 0;
    
    datosGPS->latitud = 0.00;
    datosGPS->longitud = 0.00;
    datosGPS->rumbo = 0;
    datosGPS->velocidad = 0;
    
    datosGPS->isValid = NoFix;
    datosGPS->isProcessed = false;
    datosGPS->isNewData = false;
}

void gpsFillData(gpsDatos_t *datosGPS){
    uint i = 0;
    for(i = 0; i < lenbuff; i++){   // busca en que lugar de la cadena esta el caracter $
        if('$' == bufferSerial[i]){ // dado que el CHK es correcto, el caracter $ de seguro 
            break;                  // esta en la posicion [0]
        }
    }
    if('R' == bufferSerial[i + 3] && 'M' == bufferSerial[i + 4] && 'C' == bufferSerial[i + 5]){
        parseRMC(datosGPS);
    }
    if('G' == bufferSerial[i + 3] && 'G' == bufferSerial[i + 4] && 'A' == bufferSerial[i + 5]){
    }
    cleanBuff();    
}

//** FUNCIONES PRIVADAS **//

static void parseRMC(gpsDatos_t *datosGPS){
    //$GPRMC,022831.000,A,3410.1873,S,07124.3823,W,0.12,77.63,150421,,,A*5F
    //012345678901234567890123456789012345678901234567890123456789012345678
    uint8_t comas[12];
    uint i = 0;
    uint8_t countComa = 0;
    float_t grados = 0;
    uint32_t minutosDecimal = 0;

    //busca en que posicion de la cadena estan los separadores de campo ','
    for(i = 0; i < lenbuff; i++){
        if(',' == bufferSerial[i]){
            comas[countComa++] = i;
        }
        if('*' == bufferSerial[i] || '\0' == bufferSerial[i]){
            break;
        }
    }
    //Calculo de la validez del datos
    i = comas[1] + 1;
    if('A' == bufferSerial[i]){
        datosGPS->isValid = Fix3D;
    }
    else{
        datosGPS->isValid = NoFix;
    }


    //Calculo de la hora
    i = comas[0] + 1;
    datosGPS->hora = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //Hora
    datosGPS->minuto = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //minutos
    datosGPS->segundo = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //segundos

    
    //calculo Latitud
    i = comas[2] + 1;
    grados = (float_t)(convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++])); //grados
    while(bufferSerial[i] != ','){
        if(bufferSerial[i] != '.'){
            minutosDecimal = 10 * minutosDecimal +  (convertCharToInt(bufferSerial[i]));
        }
        i++;        
    }
    datosGPS->latitud = grados + (float_t)minutosDecimal/600000;//  (float_t)minutosDecimal/6000;
    // Verificación hemisferio
    i = comas[3] + 1;
    if('S' == bufferSerial[i]){
        datosGPS->latitud *= -1;
    }

    //Calculo Longitud
    i = comas[4] + 1;
    grados = 0;
    while(i < comas[4] + 4){
        grados = 10 * grados + convertCharToInt(bufferSerial[i]);
        i++;
    }
    minutosDecimal = 0;
    while(bufferSerial[i] != ','){
        if(bufferSerial[i] != '.'){
            minutosDecimal = 10 * minutosDecimal +  (convertCharToInt(bufferSerial[i]));
        }
        i++;        
    }
    datosGPS->longitud = grados + (float_t)minutosDecimal/600000;
    //$GPRMC,022831.000,A,3410.1873,S,07124.3823,W,0.12,77.63,150421,,,A*5F
    //Calculo signo longitud
    i = comas[5] + 1;
    if('W' == bufferSerial[i]){
        datosGPS->longitud *= -1;
    }

    /*  QUEDA PENDIENTE CALCULAR:
     *      VELOCIDAD
            COG (QUE ES EL RUMBO DE LA POSICION)
            VARIACION MAGNETICA
    */

    //Calculo de la fecha
    i = comas[8] + 1;
    datosGPS->dia = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //Dia
    datosGPS->mes = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //Mss
    datosGPS->anio = convertCharToInt(bufferSerial[i++]) * 10 + convertCharToInt(bufferSerial[i++]); //Año

    datosGPS->isProcessed = false;
    
}

/*
El calculo del checksum es la sumatoria de or exclusivo de los valores
comprendidos entre $ y *
*/
static bool calcCHK(char *s){
    
    uint8_t c = 0;
    uint8_t d = 0;
    if ('$' == *s){
        ++s;
    }
    
	while ('\0' != *s && '*' != *s){
        c ^= *s++;
    }
		
    d = (convertCharToInt(*(s+1)) * 16) + convertCharToInt(*(s+2));
    if(c == d){
        return true;
    }
    return false; 
}

static uint8_t convertCharToInt(char data){
    uint8_t i = 0;
    if( data >= 'A' && data <= 'F'){
        i = data - 'A' + 10;
    }
    if( data >= '0' && data <= '9'){
        i = data - '0';
    }
    return i;
}

static void cleanBuff(){
  int i = 0;
  while(i < lenbuff){
    bufferSerial[i++] = 0;
  }
  xbuff = 0;
}


#include <sapi_gpsParser.h>

const uint16_t lenbuff = 500;

static char bufferSerial[lenbuff];
int xbuff = 0;
bool flagCommand = false;


//bool 
String getDataGPS(){
    if(calcCHK(&bufferSerial[0])){
        return bufferSerial;
    }
    return "nada";
}

        

bool getDataSerial(Stream &puertoSerie){
    char charData;
    if(puertoSerie.available()){
        charData = char(puertoSerie.read());
        switch (charData){
            case '\r': break;
            case '\n': return true;
                       break;
            default:   bufferSerial[xbuff++] = charData;
                       if(xbuff >= lenbuff){
                           xbuff = 0;
                       }
                       break;
        }
    }
    return false;
}

/*
El calculo del checksum es la sumatoria de or exclusivo de los valores
comprendidos entre $ y *
*/
bool calcCHK(char *s){
    
    uint8_t c = 0;
    uint8_t d = 0;
    if (*s == '$'){
        ++s;
    }
    
	while (*s != '\0' && *s != '*'){
        c ^= *s++;
    }
		
    d = (convertCharToInt(*(s+1)) * 16) + convertCharToInt(*(s+2));
    if(c == d){
        return true;
    }
    return false; 
}

uint8_t convertCharToInt(char data){
    uint8_t i = 0;
    if( data > '9'){
        i = data - 'A' + 10;
    }
    else{
        i = data - '0';
    }
    return i;
}

/*
uint8_t convertCharToHex(char *data){
    
}
*/
void cleanBuff(){
  int i = 0;
  while(i < lenbuff){
    bufferSerial[i++] = 0;
  }
  xbuff = 0;
}


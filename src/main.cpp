#include <Arduino.h>
#include <sapi.h>
#include <sapi_delay.h>
#include <sapi_gpsParser.h>

/** @param VersionFW Definición de la versión del FW del sistema **/
static const char *FW_VERSION = ">RVR_sfmGPS210412B0.1<";


/*
Creación de definiciones 
*/
#define ledGPS 4                // El led de Estatus del GPS esta conectado al GPIO 4
#define gpsDebugButtons 13      // Para efectos de hacer ECHO de la información recibida por el GPS
                                // se utiliza un botón para habilitar el proceso de impresion.
#define debugSerialPort Serial  // Defino el nombre del puerto serie para facilidad de entendimiento de uso y origen
#define gpsSerialPort Serial2   //
#define timeToggleLedGPS 500    // defino el tiempo para hacer blink del ledGPS en 500 mS


/*Creacion de enum para los estados de la MEF*/
typedef enum{
    GPSEstadoApagado,
    GPSEstadoPosicionando,
    GPSEstadoPosicionado
} fsmLedGPS_t;

fsmLedGPS_t estatusLedGPS;      // Se instancia una variable para ser usada en la maquina de estados
gpsDatos_t datosGPS;            // Se instancia una estructura que contiene las propiedades de una coordenada GPS

delay_t delayToggleLedGPS;      // Se instancia una variable para uso en Delay No Bloqueante!

bool_t estatusIgnicionMovil = true; /* Variable ficticia para darle sentido al cambio de estado de la MEF
                                       en el estado "GPSEstadoPosicionado".
                                       A futuro el valor de esta variable estara determinado
                                       por la ignicion del vehiculo y tendra los debunce correspondientes.
                                    */

bool_t GPSPowerSafeMode();      // Funcion ficticia que se encargara de poner el modulo GPS en modo dormido
void systemPowerSafeMode();     // Funcion ficticia que se encargara de poner el sistema en modo dormido
                                // para bajar el consumo.

void updateGPSLedStatus(fsmLedGPS_t *estatusLed); //Funcion que controla la MEF

void setup() {
    /*se inicializan los puertos series en los baudios requeridos para sus funciones*/
    debugSerialPort.begin(115200);
    gpsSerialPort.begin(9600);

    gpsDatosInit(&datosGPS);    // Se inicializa la variable que contiene los datos de cada coordenada

    estatusLedGPS = GPSEstadoApagado;   // Se inicia la MEF en estado Apagado el cual es su estado normal 
                                        // al conectarse la primera vez o al despertar del modo bajo consumo

    delayInit(&delayToggleLedGPS,timeToggleLedGPS); // Se inicializa el Delay No Bloqueante a 500 mS

    /* Se inicializan los GPIO como entradas y salidas segun su uso*/
    pinMode(gpsDebugButtons, INPUT_PULLDOWN);
    pinMode(ledGPS, OUTPUT);


    digitalWrite(ledGPS,LOW);   // Al arrancar el progra se apaga el ledGPS.

    debugSerialPort.println(FW_VERSION); // Se imprime la version del FW.
}

void loop() {

    updateGPSLedStatus(&estatusLedGPS); // Se actualiza el estado de la MEF en tiempos de maquina.
    
    if(gpsNewData(gpsSerialPort)){  // llama a funcion booleana que retorna true cuando hay una cadena GPS nueva
        
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /*  Si está presionado el boton del debug de datos GPS
            se imprime los datos recibidos
            esta funcion debe ser llamada antes de gpsFillData() para que se alcance a imprimir el string RAW recibido.
        */
        if(digitalRead(gpsDebugButtons)){
            debugSerialPort.println(getDataGPS());
        }
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /* Dado que se recibio una cadena GPS llama funcion que llena los datos de la coordenada*/
        gpsFillData(&datosGPS);  
        
        /*  Si el dato es nuevo y no ha sido procesado, imprime los detalles de la posicion.
            Tambien se actualiza el estado isProcessed para que no vuelva a imprimir hasta un nuevo dato.
        */
        if(!datosGPS.isProcessed){
    
            datosGPS.isProcessed = true;
            datosGPS.isNewData = true;

            debugSerialPort.print("Validez: "); // 1 posición FIX valida, 0 posicion no valida.
            debugSerialPort.print(datosGPS.isValid);
            debugSerialPort.print(" - FechaHora: ");
            debugSerialPort.print(datosGPS.dia);
            debugSerialPort.print("/");
            debugSerialPort.print(datosGPS.mes);
            debugSerialPort.print("/");
            debugSerialPort.print(datosGPS.anio);
            debugSerialPort.print(" ");
            debugSerialPort.print(datosGPS.hora);
            debugSerialPort.print(":");
            debugSerialPort.print(datosGPS.minuto);
            debugSerialPort.print(":");
            debugSerialPort.print(datosGPS.segundo);
            debugSerialPort.print(" - Latitud: ");
            debugSerialPort.print(datosGPS.latitud,5);
            debugSerialPort.print(" - Longitud: ");
            debugSerialPort.print(datosGPS.longitud,5);
            debugSerialPort.println("");        
            }
        
        
    }
}

/*Esta MEF solo controla el estado de apagado, transicion y encendido del ledGPS*/
void updateGPSLedStatus(fsmLedGPS_t *estatusLed){
    static bool_t flagEstatusLED = true; //Variable estatica para encender/apagar el led una sola vez por ocasion.
    static bool_t flagToggleLed = false; //Variable estatica para acompañar el estado de cambio del led.
    switch (*estatusLed)
    {
    case GPSEstadoApagado: // Inicia en estado Apagado
        if(datosGPS.isNewData){ 
            estatusLedGPS = GPSEstadoPosicionando;  // Si hay un dato GPS nuevo significa que desperto y debemo
        }                                           // pasar a modo Posicionando
        else{
            if(flagEstatusLED){                     // Si no se han recibido datos nuevos se apaga el ledGPS 1 vez.
                flagEstatusLED = false;
                digitalWrite(ledGPS,LOW);
                flagToggleLed = false;
            }
        }
        break;
    case GPSEstadoPosicionando:                     // mientras no se reciba una posicion valida
        if(datosGPS.isValid){                       // el led estara blinkeando.
            flagToggleLed = true;                   // y cuando se recibe una posicion valida
            digitalWrite(ledGPS, HIGH);             // enciende el ledGPS 1 vez
            estatusLedGPS = GPSEstadoPosicionado;   // y cambia el estado a Posicionado.
        }
        else{
            if(delayRead(&delayToggleLedGPS)){
                if(flagToggleLed){
                    digitalWrite(ledGPS,LOW);
                    flagToggleLed = false;
                }
                else{
                    digitalWrite(ledGPS,HIGH);
                    flagToggleLed = true;
                }
            }
        }
    break;
    case GPSEstadoPosicionado:                      // mientras este la ignicion detectada
        if(estatusIgnicionMovil){                   // analizara si el dato es valido
            if(datosGPS.isValid == 0){              // y cuando se dejen de recibir datos correctos
                estatusLedGPS = GPSEstadoPosicionando;  // se cambia a estado posicionando para blinkear.
            }
        }
        else{
            estatusLedGPS = GPSEstadoApagado;       // cuando detecta que el vehiculo se apaga.
            if(GPSPowerSafeMode()){                 // apaga el modulo GPS,
                digitalWrite(ledGPS,LOW);           // apaga el led,
                flagToggleLed = false;              // y pone el sistema en modo bajo consumo
                systemPowerSafeMode();              // en espera que lo despierte la interrupcion de encendido. 
            }
        }
    default:
        break;
    }
}

bool_t GPSPowerSafeMode(){
    return false;
}
void systemPowerSafeMode(){
}
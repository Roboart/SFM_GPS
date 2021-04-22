# Bilioteca GPS Quectel L80-r o L96-M33

Biblioteca para utilizar un Módulo GPS Quectel con ESP32.
## Struct gpsDatos_t

Estructura que contiene los datos de una coordenada GPS.

```c
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
```
## gpsDatosInit();

Función para inicializar los datos de la estructura de datosGPS.

```c
void gpsDatosInit(gpsDatos_t *datosGPS);
```

## bool gpsNewData(Stream &puertoSerie);

Función que verifica si han llegado coordenadas nuevas desde el módulo GPS.

&puertoSerie: Recibe como stream la referencia del puerto serie usado en el módulo ESP32.
```c
bool gpsNewData(Stream &puertoSerie);
```
### ejemplo:

```c
if(gpsNewData(gpsSerialPort)){
	/*Ejecutar código si es que se recibió un paquete de datos GPS válido*/
}
```

## String getDataGPS();

Función que retorna como String el paquete de datos GPS recibido.

```c
String getDataGPS();
```

### ejemplo:



```c
if(gpsNewData(gpsSerialPort)){
	debugSerialPort.println(getDataGPS());
}
```

## void gpsFillData(gpsDatos_t *datosGPS);

Función que identifica el tipo de paquete de datos GPS y actualiza los valores de la estructura de datosGPS.

```c
void gpsFillData(gpsDatos_t *datosGPS);
```

### ejemplo:


```c
gpsDatos_t datosGPS;
...
While(true){
	...
	gpsFillData(&datosGPS);
	...
}
```


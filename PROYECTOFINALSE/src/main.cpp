#include <Arduino.h>
#include "apwifiesp32.h"
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 // 4 blocks
#define CS_PIN 5

#define carga 35
#define ledbajo 19
#define lednormal 32
#define ledalto 33

#define botonCow 34
#define botonHorse 4
#define botonBird 21
#define botonDog 12
#define botonSheep 13
#define botonMonkey 14
#define botonMiau 15
#define boton1 27
#define boton2 26

#define TIMER_INTERVAL_MS 5000 // Intervalo de tiempo para la interrupcion
#define TIMER_DIVIDER 80       // Divisor para una frecuencia de 1 MHz
MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
hw_timer_t *timer = NULL; // Define el tiempo del temporizador
// sincroniza el bucle principal y la ISR cuando se modifique una variable compartida
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
long tiempoAnterior = 0;
long intervalo = 1000;

int estadoSensorCow;
bool estadoBotonAnteriorSensorCow;

int estadoSensorHorse;
bool estadoBotonAnteriorSensorHorse;

int estadoSensorBird;
bool estadoBotonAnteriorSensorBird;

int estadoSensorDog;
bool estadoBotonAnteriorSensorDog;

int estadoSensorSheep;
bool estadoBotonAnteriorSensorSheep;

int estadoSensorMonkey;
bool estadoBotonAnteriorSensorMonkey;

int estadoSensorMiau;
bool estadoBotonAnteriorSensorMiau;
int estadoSensor1;
bool estadoBotonAnteriorSensor1;
int estadoSensor2;
bool estadoBotonAnteriorSensor2;

const int tiempo = 8;
const int tiempoAntirebote = 10;

int estadoBotonCow;
int estadoBotonHorse;
int estadoBotonBird;
int estadoBotonDog;
int estadoBotonSheep;
int estadoBotonMonkey;
int estadoBotonMiau;
int estadoBoton1;
int estadoBoton2;

HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

void Estadocarga(int pin);
void Estadocarga(int pin)
{
    int sensor = analogRead(pin);
    Serial.println("BIT: ");
    Serial.println(sensor);
    float voltaje = sensor * (3.3 / 4096) * 1.5454;
    Serial.println("VOLTAJE:");
    Serial.println(voltaje);
    if (voltaje > 0 && voltaje <= 0.63)
    {
        ledcWriteTone(0, 261);
        digitalWrite(ledbajo, HIGH);
        digitalWrite(lednormal, LOW);
        digitalWrite(ledalto, LOW);
    }
    else if (voltaje > 0.63 && voltaje <= 2.1)
    {
        Serial.println("1");
        ledcWriteTone(0, 0);
        digitalWrite(ledbajo, HIGH);
        digitalWrite(lednormal, LOW);
        digitalWrite(ledalto, LOW);
    }
    else if (voltaje > 2.1 && voltaje <= 3.15)
    {
        Serial.println("2");
        ledcWriteTone(0, 0);
        digitalWrite(ledbajo, HIGH);
        digitalWrite(lednormal, HIGH);
        digitalWrite(ledalto, LOW);
    }
    else if (voltaje > 3.15)
    {
        Serial.println("3");
        ledcWriteTone(0, 0);
        digitalWrite(ledbajo, HIGH);
        digitalWrite(lednormal, HIGH);
        digitalWrite(ledalto, HIGH);
    }
}
void IRAM_ATTR timerInterrupt()
{
    portENTER_CRITICAL_ISR(&timerMux); // Declara el inicio de la interrupcion
    Estadocarga(carga);
    portEXIT_CRITICAL_ISR(&timerMux); // Declara que la interrupcion termino
}
boolean antiRebote(int pin)
{
    int contador = 0;
    boolean estado;         // guarda el estado del boton
    boolean estadoAnterior; // guarda el ultimo estado del boton

    do
    {
        estado = digitalRead(pin);
        if (estado != estadoAnterior)
        {                 // comparamos el estado actual
            contador = 0; // reiniciamos el contador
            estadoAnterior = estado;
        }
        else
        {
            contador = contador + 1; // aumentamos el contador en 1
        }
        delay(1);
    } while (contador < tiempoAntirebote);
    return estado;
}
void setup()
{
    initAP("espAP", "123456789"); // nombre de wifi a generarse y contrasena
    mySerial.begin(9600, SERIAL_8N1, 16, 17);
    if (myDFPlayer.begin(mySerial))
    {
        Serial.println("DFPlayer Mini inicializado correctamente.");
    }
    else
    {
        Serial.println("Error al inicializar el DFPlayer Mini.");
    }
    myDFPlayer.setTimeOut(500);
    myDFPlayer.volume(30);
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);

    ledMatrix.begin();         // initialize the LED Matrix
    ledMatrix.setIntensity(0); // set the brightness of the LED matrix display (from 0 to 15)
    ledMatrix.displayClear();  // clear LED matrix display
    pinMode(botonCow, INPUT);
    pinMode(botonHorse, INPUT_PULLUP);
    pinMode(botonBird, INPUT_PULLUP);
    pinMode(botonDog, INPUT_PULLUP);
    pinMode(botonSheep, INPUT_PULLUP);
    pinMode(botonMonkey, INPUT_PULLUP);
    pinMode(botonMiau, INPUT_PULLUP);
    pinMode(boton1, INPUT_PULLUP);
    pinMode(boton2, INPUT_PULLUP);

    /*
    ledcSetup(0, 2000, 8);
    ledcAttachPin(22, 0);
    pinMode(ledbajo, OUTPUT);
    pinMode(lednormal, OUTPUT);
    pinMode(ledalto, OUTPUT);
    timer = timerBegin(1, TIMER_DIVIDER, true);         // Inicializa el timer
    timerAttachInterrupt(timer, &timerInterrupt, true); // Declara la interrupcion
    // Define el tiempo en que se activa la interrupcion
    timerAlarmWrite(timer, TIMER_INTERVAL_MS * 1000, true);
    timerAlarmEnable(timer); // Habilita la alarma del timer*/
}

void loop()
{
    loopAP();
    estadoBotonCow = !digitalRead(botonCow);
    estadoBotonHorse = !digitalRead(botonHorse);
    estadoBotonBird = !digitalRead(botonBird);
    estadoBotonDog = !digitalRead(botonDog);
    estadoBotonSheep = !digitalRead(botonSheep);
    estadoBotonMonkey = !digitalRead(botonMonkey);
    estadoBotonMiau = !digitalRead(botonMiau);
    estadoBoton1 = !digitalRead(boton1);
    estadoBoton2 = !digitalRead(boton2);
    if (estadoBotonCow != estadoBotonAnteriorSensorCow)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonCow))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Cow"); // display text
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 1);
        }
    }
    estadoBotonAnteriorSensorCow = estadoSensorCow;
    //  ********************* HORSE **********************************************

    if (estadoBotonHorse != estadoBotonAnteriorSensorHorse)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonHorse))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Horse");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 2);
        }
    }

    estadoBotonAnteriorSensorHorse = estadoSensorHorse; // guardamos el estado del boton

    //  *********************** BIRD  ***********************************************

    if (estadoBotonBird != estadoBotonAnteriorSensorBird)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonBird))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Bird");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 3);
        }
    }

    estadoBotonAnteriorSensorBird = estadoSensorBird;

    //  *********************** DOG ***********************************************

    if (estadoBotonDog != estadoBotonAnteriorSensorDog)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonDog))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Dog");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 4);
        }
    }

    estadoBotonAnteriorSensorDog = estadoSensorDog;

    //  *********************** SHEEP ***********************************************

    if (estadoBotonSheep != estadoBotonAnteriorSensorSheep)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonSheep))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Sheep");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 5);
        }
    }

    estadoBotonAnteriorSensorSheep = estadoSensorSheep;

    //  *********************** MONKEY ***********************************************

    if (estadoBotonMonkey != estadoBotonAnteriorSensorMonkey)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonMonkey))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Monkey");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 6);
        }
    }

    estadoBotonAnteriorSensorMonkey = estadoSensorMonkey;

    if (estadoBotonMiau != estadoBotonAnteriorSensorMiau)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(botonMiau))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Cat");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 7);
        }
    }

    estadoBotonAnteriorSensorMiau = estadoSensorMiau;
    if (estadoBoton1 != estadoBotonAnteriorSensor1)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(boton1))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Elephant");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 8);
        }
    }

    estadoBotonAnteriorSensor1 = estadoSensor1;
    if (estadoBoton2 != estadoBotonAnteriorSensor2)
    {
        // si hay cambio con respeto al estado
        if (antiRebote(boton2))
        {
            ledMatrix.displayClear();
            ledMatrix.setTextAlignment(PA_CENTER);
            ledMatrix.print("Rooster");
            // checamos  si esta presionado y si lo esta
            myDFPlayer.playFolder(15, 9);
        }
    }
    estadoBotonAnteriorSensor2 = estadoSensor2;
}

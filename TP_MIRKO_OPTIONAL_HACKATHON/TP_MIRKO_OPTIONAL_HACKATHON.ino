/* ------------------------------------------------------------------------------------------------------ /

  Código: Hackathon

  Autores: Elian Umansky, Uriel Slavkin, Theo E. Scladman, Eitan Shai Langman, Facundo S. Seijas
  Materia: ST (Seminario De Informatica Y Telecomunicaciones)
  Trabajo: TP2 5B opcional hackathon

  Descripción:
  (...)

  /* ----------------------------------------------------------------------------------------------------- */


#include "DHT.h"               // librerias de los tutoriales para el display, el dht 11, y el reloj
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <ESP32Time.h>


#define SCREEN_WIDTH 128 // ancho en pixels
#define SCREEN_HEIGHT 64 // altura en pixels

#define PIN_LED 25

#define PIN_BOTON_SUBIR 35 //pines de los botones y sus flags
#define PIN_BOTON_BAJAR 34
bool flagSubir, flagBajar;


#define PANTALLA_MAIN 1    //estados de la maquina
#define ESPERA_MINUTOS 2
#define PANTALLA_MINUTOS 3
#define ESPERA_HORAS 4
#define PANTALLA_HORAS 5
#define ESPERA_MAIN 6
#define REGANDO  7

int estado = 1;  //inicia en el estado 1

// Inicializar el display y el dht
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);



int segs = 0, mins = 0, hours;
int periodo = 1, riego = 5;//tiempro entre regados y duracion de regado


int milisSegs;
int milisMins;

void setup() 
{
  
  Serial.begin(9600);
  
  pinMode(PIN_BOTON_SUBIR, INPUT_PULLUP);    
  pinMode(PIN_BOTON_BAJAR, INPUT_PULLUP);  
  pinMode(PIN_LED, OUTPUT);  
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);

}

void loop() 
{

  if(millis() - milisMins >= 20000)  //milis -> cuenta los ms desde que empieza el programa
  {
    mins = mins + 1;                     //cada xxx ctad de milis, sumo 1 minuto o segundo 
    milisMins = millis();
  }

  if(millis() - milisSegs >= 1000)
  {
    segs = segs + 1;
    milisSegs = millis();
  }
  
  switch(estado)
  {
    
    case PANTALLA_MAIN: //pantalla principal que muestra cuanto falta para regar   ------------------------------------ 1 -----------------------------------
   
      {      
     
        
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print("Faltan:");
      int tiempoQueFalta = periodo-mins;  // el tiempo que falta es el periodo entre riegos - la ctad de minutos que ya pasaron
      display.println(tiempoQueFalta);  
      display.println("Min");                   
      display.display();
      Serial.println("estado 1"); //imprimo el estado   
      Serial.println(segs);
      
      if(tiempoQueFalta <= 0)   //cambio al estado regando cuando no quede mas tiempo, es decir cuando tiempo pasado = tiempo entre riegos
      {
        estado = REGANDO;
        segs = 0;                          // empiezo a contar segundos desde que empece a regar 
        digitalWrite(PIN_LED, HIGH);    //prendo el led
        display.setCursor(0, 10);        
        display.clearDisplay();
        display.print("REGANDO");        
        display.display();
      }
             
      if(digitalRead(PIN_BOTON_BAJAR) == LOW && digitalRead(PIN_BOTON_SUBIR) == LOW)// si apreto los botones cambio al estado de espera
      {
        estado = ESPERA_MINUTOS; 
      }
      
    
}
    break;


    case ESPERA_MINUTOS:  //estado esperando la liberacion de los botones para pasar a la otra pantalla ------------------------------- 2 --------------------------------
         
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && digitalRead(PIN_BOTON_SUBIR) == HIGH)//si suelto los botones cambio al estado siguiente
      {
        estado = PANTALLA_MINUTOS;
      }
    break;

      
    case PANTALLA_MINUTOS: //en esta pantalla se muestra el periodo para cambiarse con los botones ------------------------------- 3 -------------------------------

      Serial.println("3");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.print("Priodo: ");
      display.println(periodo);
      display.display();    
      
      if(digitalRead(PIN_BOTON_BAJAR) == LOW) // si pareto el boton, sube el flag para evitar que se cuenten varios clicks
      {
        flagBajar = HIGH;
      }
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && flagBajar == HIGH) //si suelto el boton, se cuenta un click y cambia el valor de la variable periodo
      {
        flagBajar = LOW;                       //reseteo el flag
        periodo = periodo + 1;

      }    

      if(digitalRead(PIN_BOTON_SUBIR) == LOW) //lo mismo con el otro boton
      {
        flagSubir = HIGH;
      }
      if(digitalRead(PIN_BOTON_SUBIR) == HIGH && flagSubir == HIGH)
      {
        flagSubir = LOW;
        periodo = periodo - 1;
      }    
      
      if(digitalRead(PIN_BOTON_BAJAR) == LOW && digitalRead(PIN_BOTON_SUBIR) == LOW)  //cambio al estado de espera
      {
        estado = ESPERA_HORAS;
      }
    break;


    case ESPERA_HORAS: //estado de espera para ir a la proxima pantalla  --------------------------------------- 4 -----------------------------------
    
      Serial.println("4");
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && digitalRead(PIN_BOTON_SUBIR) == HIGH) //cambio cuando suelto los dos botones
      {
        estado = PANTALLA_HORAS;
      }
    
    break;   
  

      case PANTALLA_HORAS: //estado para cambiar la duracion del riego con los botones  --------------------------------------- 5 -----------------------------------
    
      Serial.println("5");

      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Duracion ");
      display.println("de riego: ");
      display.print(riego);
      display.display();

      if(digitalRead(PIN_BOTON_BAJAR) == LOW) // si pareto el boton, sube el flag para evitar que se cuenten varios clicks
      {
        flagBajar = HIGH;
      }
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && flagBajar == HIGH) //si suelto el boton, se cuenta un click y cambia el valor de la variable riego
      {
        flagBajar = LOW;                       //reseteo el flag
        riego = riego + 1;

      }    

      if(digitalRead(PIN_BOTON_SUBIR) == LOW) //lo mismo con el otro boton
      {
        flagSubir = HIGH;
      }
      if(digitalRead(PIN_BOTON_SUBIR) == HIGH && flagSubir == HIGH)
      {
        flagSubir = LOW;
        riego = riego - 1;
      }    
      
      if(digitalRead(PIN_BOTON_BAJAR) == LOW && digitalRead(PIN_BOTON_SUBIR) == LOW)  //cambio al estado de espera
      {
        estado = ESPERA_MAIN;
      }

    
    break;   
  
      case ESPERA_MAIN: //estado de espera para volver a la pantalla principal --------------------------------------- 6 -----------------------------------
    
      Serial.println("6");
      if(digitalRead(PIN_BOTON_BAJAR) == HIGH && digitalRead(PIN_BOTON_SUBIR) == HIGH) //vuelvo a la main cuando suelto los dos botones
      {
        estado = PANTALLA_MAIN;
      }
    
    break;   

    case REGANDO:   


          if (segs > riego)       //cuando los segundos pasados desde que empieza el riego superan la duracion, vuelvo al estado principal
          { 
            estado = PANTALLA_MAIN;
            segs = 0;
            mins = 0;
            Serial.print("DEJO DE REGAR");
            digitalWrite(PIN_LED, LOW);             
          }
          

    break;

    
  }

}

//#include "FS.h"
#include <Arduino.h>
#include "Imagenes.h"
#include <SPI.h>
#include <TFT_eSPI.h>     
#include "RTClib.h"
#include <cstdlib>//libreria que usamos para convertir caracteres a enteros

int a=96;
#define Default 90
TFT_eSPI tft = TFT_eSPI();
uint8_t MenuActual=90,PantallaActual=90;//los igualamos a valores inalcanzables para que no arranquen activando nada
bool verificados[4]={false,false,false,false};//son para comprobar si se setearon los valores a configurar
enum{ALARMA,PROGRAMADA,EXISTENTES,CONFIGURACION};
enum{PASTILLA_1, PASTILLA_2, PASTILLA_3, PASTILLA_4};
enum{DURACION,INTERVALO,PASTILLA,CONFIRMADO,ATRAS};

typedef struct {//definicion de la estructura para las alarmas
  DateTime Proxima;
  uint8_t DuracionDias=-1;
  uint8_t Pastilla=-1;
  uint8_t Intervalo=-1;
  uint8_t CantidadPastillas=-1;//sera el resultado de la division de la cantidad de dias entre las horas del intervalo
  bool Activa=false;//cuando la alarma cumpla su ciclo se desactivara
}AlarmaVar;

typedef struct {//definicion de la estructura para las alarmas
  DateTime HoraInicio;
  uint8_t DuracionDias;
  int8_t Pastilla[4];
  int8_t Intervalo;
  bool Activa=false;//cuando la alarma cumpla su ciclo se desactivara
}ProgramadaVar;

AlarmaVar Alarma[4];
ProgramadaVar Programadas[8];
//DateTime aver = {6,2,0};

/*void touch_calibrate(){
 
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  //Serial.println(); //Serial.println();
  //Serial.println("// Use this calibration code in setup():");
  //Serial.print("  uint16_t calData[5] = ");
  //Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    //Serial.print(calData[i]);
    if (i < 4) //Serial.print(", ");
  }

  //Serial.println(" };");
  //Serial.print("  tft.setTouch(calData);");
  //Serial.println(); //Serial.println();

  tft.fillScreen(TFT_BLACK);
 
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to //Serial port.");

  delay(4000);
}
*/
void setup() {
  tft.init();
 Serial.begin(9600);

  // Set the rotation before we calibrate
  tft.setRotation(2);//la parte de los pines es la de arriva
  //touch_calibrate();
  uint16_t calData[5] = { 258, 3487, 365, 3541, 2 };
  tft.setTouch(calData);

  // Calibrate the touch screen and retrieve the scaling factors
  //touch_calibrate();
 
 

  // Draw keypad background
  //tft.fillScreen(TFT_DARKGREY);

  // Draw number display area and frame
  /*tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  tft.drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);
*/
}

void botonAtras(){
  TFT_eSprite atras = TFT_eSprite(&tft);

  atras.setSwapBytes(true);
  atras.setColorDepth(8);//cantidad de color que va a tener(8 bits vamos a usar nosotros y nos funciona bien)
//  flecha.fillSprite(TFT_TRANSPARENT);
  atras.createSprite(40,25);//creamos el sprite en memoria especificandole su tamaño(tiene que ser minimo el mismo tamaño que la imagen, sino no la mostrara entera)
  atras.pushImage(0,0,40,25,flecha);// las coordenadas tienen que ser 0,0 porque son las coordenadas del sprite si no son 0,0 se va a dibujar desfasado(cambiarlo para probar y ver que es asi) y el tamaño de la imagen que tiene que ser el mismo que en createSprite y que imagen le vamos a estar asignado al sprite(esta funcion carga la imagen dentro del sprite)
  atras.pushSprite(0,0,TFT_TRANSPARENT);//envia la imagen a la pantalla en las coordenadas que le pasemos y el color de fondo que tendra el sprite
  atras.deleteSprite();
}

void dibujarRenglon(bool dosPuntos=false){
  TFT_eSprite img = TFT_eSprite(&tft);

  img.setSwapBytes(true);
  img.setColorDepth(8);//cantidad de color que va a tener(8 bits vamos a usar nosotros y nos funciona bien)
//  flecha.fillSprite(TFT_TRANSPARENT);
  img.createSprite(214,40);//creamos el sprite en memoria especificandole su tamaño(tiene que ser minimo el mismo tamaño que la imagen, sino no la mostrara entera)
  img.pushImage(0,0,214,40,displayBlanco);// las coordenadas tienen que ser 0,0 porque son las coordenadas del sprite si no son 0,0 se va a dibujar desfasado(cambiarlo para probar y ver que es asi) y el tamaño de la imagen que tiene que ser el mismo que en createSprite y que imagen le vamos a estar asignado al sprite(esta funcion carga la imagen dentro del sprite)
  img.pushSprite(13,60,TFT_TRANSPARENT);//envia la imagen a la pantalla en las coordenadas que le pasemos y el color de fondo que tendra el sprite
  if(dosPuntos){
    tft.setTextSize(6);
    tft.setTextColor(TFT_RED);
    tft.drawString(":",106,60);
  }
  img.deleteSprite();
  
}

void dibujarTick(int fila){
  TFT_eSprite img = TFT_eSprite(&tft);

  img.setSwapBytes(true);
  img.setColorDepth(8);//cantidad de color que va a tener(8 bits vamos a usar nosotros y nos funciona bien)
  img.createSprite(45,45);//creamos el sprite en memoria especificandole su tamaño(tiene que ser minimo el mismo tamaño que la imagen, sino no la mostrara entera)
  img.pushImage(0,0,45,45,tick);// las coordenadas tienen que ser 0,0 porque son las coordenadas del sprite si no son 0,0 se va a dibujar desfasado(cambiarlo para probar y ver que es asi) y el tamaño de la imagen que tiene que ser el mismo que en createSprite y que imagen le vamos a estar asignado al sprite(esta funcion carga la imagen dentro del sprite)
  img.pushSprite(205,70+45*fila,TFT_TRANSPARENT);//envia la imagen a la pantalla en las coordenadas que le pasemos y el color de fondo que tendra el sprite
  img.deleteSprite();
}

void dibujarCalendario(){
  TFT_eSprite img = TFT_eSprite(&tft);

  img.setSwapBytes(true);
  img.setColorDepth(8);//cantidad de color que va a tener(8 bits vamos a usar nosotros y nos funciona bien)
  img.createSprite(90,90);//creamos el sprite en memoria especificandole su tamaño(tiene que ser minimo el mismo tamaño que la imagen, sino no la mostrara entera)
  img.pushImage(0,0,90,90,calendario);// las coordenadas tienen que ser 0,0 porque son las coordenadas del sprite si no son 0,0 se va a dibujar desfasado(cambiarlo para probar y ver que es asi) y el tamaño de la imagen que tiene que ser el mismo que en createSprite y que imagen le vamos a estar asignado al sprite(esta funcion carga la imagen dentro del sprite)
  img.pushSprite(0,0,TFT_TRANSPARENT);//envia la imagen a la pantalla en las coordenadas que le pasemos y el color de fondo que tendra el sprite
  img.deleteSprite();
}

void tecladoSemanal(uint8_t *ptr){
 TFT_eSPI_Button boton[8];                                    
 char letras[8][6] = {"L","M","X","J","V","S","D","ENTER"};//texto de los botones
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
 botonAtras();//el boton para retroceder
 tft.drawString("Seleccione los dias",5,40);
 uint8_t cord_x=36, cord_y=36, ancho_alto=65, separacion_x=36, separacion_y=5;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion;
 boolean presionado=false, apretoEnter=false;
  for(int fila=0; fila<3; fila++){//estos for anidados dibujan el teclado
    for(int columna=0; columna<3; columna++){
     
     posicion= columna + fila*3; //porsicion en el vector, a fila se la multiplica por 2 para que no repita las posiciones anteriores. Ejemplo: posicion 0+0*2 esta en la posicion 0 pero si 0+1*2 ahora va a estar en la 2
                                 //es dificil de explicar pero saca el por 2 proba y te vas a dar cuenta.  
                                     
     boton[posicion].initButtonUL(&tft, 10 + columna * (65+10), 80 + fila * (65 + 10),65, 65,TFT_WHITE, TFT_BLUE, TFT_WHITE,letras[posicion], 4);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
        if(posicion==7){
          boton[posicion].initButtonUL(&tft, 10 + columna * (65+10), 80 + fila * (65 + 10),140, 65,TFT_WHITE, TFT_RED, TFT_WHITE, letras[posicion], 2);//si es el boton de enter letra mas chica y color rojo.
          columna = 4;                          
        }
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior
     

     }
  }
 
      for(posicion=0; posicion <=8 && !apretoEnter; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==8)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)){
          boton[posicion].press(true);
          boton[posicion].drawButton(true);//si se le manda un true dibuja el boton con colores invertidos
          //Serial.println("sexo");
          delay(300); //hay que cambiarlo por una funcion con timer
          }
        presionado = tft.getTouch(&x,&y);
        //Serial.println(presionado);
        if(presionado && boton[posicion].contains(x,y) && boton[posicion].isPressed()){
          boton[posicion].drawButton();
          boton[posicion].press(false);
          //Serial.println("NO SEXO");
          delay(300); //hay que cambiarlo por una funcion con timer
          }
        if(boton[7].contains(x,y)){//si el se apreta el enter se rompe el for y sale del loop
          *ptr;
          apretoEnter=true;
        }
   }
 
  tft.fillScreen(TFT_BLUE);
  tft.drawString("siguiente",0,0);
}

void tecladoTiempo(DateTime actual){
 //&ptr->Proxima;
 DateTime *ptr=&actual;
 actual={2014,8,8,8};//actual.Proxima={2014,8,8,8};
 TFT_eSPI_Button boton[12];
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
 tft.drawString("Seleccione la hora",13,39);
 //tft.setTextSize(6);
 //tft.drawString(":",103,60);
 dibujarRenglon(true);//mide 214x40px y esta en 13,60
 botonAtras();//mide 40x25px y esta en 0,0
 char letras[12][6] = {"1","2","3","4","5","6","7","8","9","CLR","0","ENTER"};//texto de los botones
 int cord_x=13, cord_y=100, ancho=62, alto=45, separacion_x=13, separacion_y=20;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion,digito=0;
 boolean presionado=false, apretoEnter=false;
 
 //parte dibujo
  for(int fila=0; fila<4; fila++){//estos for anidados dibujan el teclado
    for(int columna=0; columna<3; columna++){
     
     posicion= columna + fila*3; //porsicion en el vector, a fila se la multiplica por 3 para que no repita las posiciones anteriores.
                                 //es dificil de explicar pero saca el por 3 proba y te vas a dar cuenta.  
                                     
     boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_BLUE, TFT_WHITE,letras[posicion], 3);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
        if(posicion==11){
          boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 2);//si es el boton de enter letra mas chica y color rojo.                            
        }
         if(posicion==9){
          boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 3);//si es el boton de enter letra mas chica y color rojo.                            
        }
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior

     }
  }
  //esta es la parte touch
      for(posicion=0; posicion <=12 && !apretoEnter; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==12)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)) boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
       
        else boton[posicion].press(false);  

        if(boton[posicion].justPressed() && digito<4){
          boton[posicion].drawButton(true);
          tft.setTextSize(4);
          tft.setTextColor(TFT_RED);
          tft.drawString(letras[posicion],70 +(digito *27),65);
          digito++;
         }
         if(boton[9].justPressed()){//si se apreto el CLEAR(boton 9) que se reinicie digito y se redibuje el Renglon 
          digito=0;
          dibujarRenglon(true);
         }
       
        if(boton[posicion].justReleased()) boton[posicion].drawButton();//dibuja el boton normal despues de ser pulsado 

        if(x<=40 && y<=25) apretoEnter=true;//si se apretaron las coordenadas que contienen el boton de Atras se rompe el for
     
        if(boton[11].contains(x,y)){//si el se apreta el enter se rompe el for y sale del loop
            apretoEnter=true;
          }
       }
}

void tecladoNumerico(void *variable,int8_t caso){
 TFT_eSPI_Button boton[12];
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);//el color del texto y del fondo del texto
 switch (caso)
 {
 case INTERVALO:
  tft.drawString("Indique cada cuanto soltar",13,39);
  break;
 
 case DURACION:
  tft.drawString("Indique la duracion en dias",13,39);
  break;
 }
 dibujarRenglon(true);//mide 214x40px y esta en 13,60
 botonAtras();//mide 40x25px y esta en 0,0
 char letras[12][6] = {"1","2","3","4","5","6","7","8","9","CLR","0","ENTER"};//texto de los botones
 int cord_x=13, cord_y=100, ancho=62, alto=45, separacion_x=13, separacion_y=20;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 uint16_t x=500,y=500,posicion,digito=0;
 char resultado[10];
 boolean presionado=false, apretoEnter=false;
 
 //parte dibujo
  for(int fila=0; fila<4; fila++){//estos for anidados dibujan el teclado
    for(int columna=0; columna<3; columna++){
     
     posicion= columna + fila*3; //porsicion en el vector, a fila se la multiplica por 3 para que no repita las posiciones anteriores.
                                 //es dificil de explicar pero saca el por 3 proba y te vas a dar cuenta.  
                                     
     boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_BLUE, TFT_WHITE,letras[posicion], 3);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
        if(posicion==11){//boton enter
          boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 2);//si es el boton de enter letra mas chica y color rojo.                            
        }
         if(posicion==9){//boton clear
          boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 3);//si es el boton de enter letra mas chica y color rojo.                            
        }
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior

     }
  }
 //esta es la parte touch
      for(posicion=0; posicion <=12 && !apretoEnter; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==12)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)) boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
       
        else boton[posicion].press(false);  

        if(boton[posicion].justPressed() && digito<4 && posicion!=11){
          boton[posicion].drawButton(true);
          tft.setTextSize(4);
          tft.setTextColor(TFT_RED);
          tft.drawString(letras[posicion],70 +(digito *27),65);
          strcat(resultado,letras[posicion]);//esta funcion agrega el caracter al string enviado  strcat(destino,caracter)
          digito++;
         }
         if(boton[9].justPressed()){//si se apreto el CLEAR(boton 9) que se reinicie digito y se redibuje el Renglon 
          digito=0;
          //resultado='0';//a revisar
          //int*variable=0;//a revisar
          dibujarRenglon(true);
         }
       
        if(boton[posicion].justReleased()) boton[posicion].drawButton();//dibuja el boton normal despues de ser pulsado 

        if(x<=40 && y<=25) apretoEnter=true;//si se apretaron las coordenadas que contienen el boton de Atras se rompe el for
     
        if(boton[11].contains(x,y)){//si el se apreta el enter se rompe el for y sale del loop
          *(int*)variable=atoi(resultado);//aca convierto el void pointer al tipo int y lo igualo a atoi que convierte en int los caracteres enviados
          verificados[caso]=true;
          apretoEnter=true;
        }
      }
     
    tft.fillScreen(TFT_OLIVE);
    tft.drawString("siguiente",0,0);
}

uint8_t configAlarma(){
 TFT_eSPI_Button boton[4];
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
 tft.drawString("COMPLETE LOS CAMPOS",6,40);
 botonAtras();//mide 40x25px y esta en 0,0
 char letras[4][26] = {"Duracion","Intervalo","Pastilla","CONFIRMAR"};//texto de los botones
 int cord_x=0, cord_y=70, ancho=240, alto=60;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion,botonActual=500;//botonActual, x e y inician en 500 para no activar el boton de atras
 boolean presionado=false, apretoEnter=false;
 
 //parte dibujo
  for(int fila=0; fila<4; fila++){//este for dibuja el menu

     posicion=fila;
     
     boton[posicion].initButtonUL(&tft, cord_x, cord_y + fila * (alto),ancho, alto,TFT_WHITE, TFT_BLUE, TFT_WHITE,letras[posicion], 3);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
       
     boton[posicion].drawButtonRectangular(false,false);//funcion personalizada que dibuja botones rectangulares, se le envia false para boton normal y true para invertido. 
                                                        //aparte se le manda el string que puede ser de mas de 9 caracteres el ultimo parametro es si dibuja el texto en el centro o desde la izq      
        if(posicion==3){
          boton[posicion].initButtonUL(&tft, 0, 250,240, 70,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 3);//si es el boton de enter letra mas grande y color rojo.
          boton[posicion].drawButtonRectangular(false,true);  // y el texto en el centro                          
        }
    if(verificados[PASTILLA])dibujarTick(PASTILLA);//si se seteo un valor y se confirmo verficado sera verdad y se dibujara el tick correspondiente
    if(verificados[DURACION])dibujarTick(DURACION);
    if(verificados[INTERVALO])dibujarTick(INTERVALO);  
  }
  //esta es la parte touch
   for(posicion=0; posicion <=4; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==4)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)){
          botonActual=posicion;
          boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
        }
       
        else boton[posicion].press(false);  

        Serial.println(posicion);
       /* if(boton[posicion].justPressed()){//si se apreto el boton dibujar invertido
          boton[posicion].drawButtonRectangular(true);
         }
       */
        if(boton[posicion].justReleased()) boton[posicion].drawButtonRectangular();//dibuja el boton normal despues de ser pulsado 

        if(x<=40 && y<=25) return ATRAS;//si se apretaron las coordenadas que contienen el boton de Atras se rompe el for

        //if(boton[3].justPressed()) return CONFIRMADO;
     
        switch(botonActual){

          case DURACION:
          boton[botonActual].drawButtonRectangular(true);
          return DURACION;
          break;

          case INTERVALO:
          boton[botonActual].drawButtonRectangular(true);
          return INTERVALO;
          break;

          case PASTILLA:
          boton[botonActual].drawButtonRectangular(true);
          return PASTILLA;
          break;

          case CONFIRMADO:
          Serial.println("CONFIRMADO");
          return CONFIRMADO;
          break;
        }
    }
}  

uint8_t menuInicio(){
 TFT_eSPI_Button boton[4];
 char letras[4][30] = {"Alarma", "Programada", "Existentes", "Config"};//texto de los botones. problema parece que la funcuion para los botones solo acepta texto de 9 caracteres
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
 tft.drawString("Elija una opcion",25,39);
 botonAtras();//el boton para retroceder
 
 uint8_t cord_x=13, cord_y=100, ancho=62, alto=45, separacion_x=13, separacion_y=20;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion,botonActual=155;//x e y en 500 para no gatillar el atras
 boolean presionado=false;
 
  for(int fila=0; fila<2; fila++){//estos for anidados dibujan el teclado
    for(int columna=0; columna<2; columna++){
     
     posicion= columna + fila*2; //porsicion en el vector, a fila se la multiplica por 3 para que no repita las posiciones anteriores.
                                 //es dificil de explicar pero saca el por 3 proba y te vas a dar cuenta.  
                                     
     boton[posicion].initButtonUL(&tft, 20 + columna * (90+20), 90 + fila * (90 + 20),90, 90,TFT_WHITE, TFT_PURPLE, TFT_WHITE, letras[posicion], 2);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior
        /*if(posicion==11){
          boton[posicion].initButtonUL(&tft, 13 + columna * (62+13), 112 + fila * (45 + 6),62, 45,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 2);//si es el boton de enter letra mas chica y color rojo.                            
        }
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior

     }*/
    }  
  }

  //esta es la parte touch
   for(posicion=0; posicion <=4 ; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==4)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)){
          botonActual=posicion;
          boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
        }
       
        else boton[posicion].press(false);  

        if(boton[posicion].justReleased()) boton[posicion].drawButton();//dibuja el boton normal despues de ser pulsado 

        if(x<=40 && y<=25) return ATRAS;//si se apretaron las coordenadas que contienen el boton de Atras se rompe el for
        

        switch(botonActual){
          case ALARMA:
          boton[botonActual].drawButton(true);
          //delay con timer2 para que la pantalla dibuje el boton
          Serial.println("ALARMA");
          return ALARMA;
          break;

          case PROGRAMADA:
          boton[botonActual].drawButton(true);
          Serial.println("PORRO");
          return PROGRAMADA;
          break;

          case EXISTENTES:
          boton[botonActual].drawButton(true);
          Serial.println("EX");
          return EXISTENTES;
          break;

          case CONFIGURACION:
          boton[botonActual].drawButton(true);Serial.println("con");
          return CONFIGURACION;
          break;
        }
    }
}

uint8_t selector1Pastilla(uint8_t*ptr){
 TFT_eSPI_Button boton[5];
 char letras[5][7] = {"PAST 1", "PAST 2", "PAST 3", "PAST 4","ENTER"};//texto de los botones. problema parece que la funcuion para los botones solo acepta texto de 9 caracteres
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.drawString("Elija UNA pastilla",20,39);
 botonAtras();//el boton para retroceder
 
 uint8_t cord_x=13, cord_y=100, ancho=62, alto=45, separacion_x=13, separacion_y=20;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion,pastillaElegida;
 boolean presionado=false, apretoEnter=false;
 
  for(int fila=0; fila<3; fila++){//estos for anidados dibujan el teclado
    for(int columna=0; columna<2; columna++){
     
     posicion= columna + fila*2; //porsicion en el vector, a fila se la multiplica por 2 para que no repita las posiciones anteriores.
                                 //es dificil de explicar pero saca el por 2 proba y te vas a dar cuenta.  
                                     
     boton[posicion].initButtonUL(&tft, 30 + columna * (75+30), 60 + fila * (75 + 10),75, 75,TFT_WHITE, TFT_BLUE, TFT_WHITE, letras[posicion], 2);//esta funcion dibuja los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
     if(posicion==4){
     boton[4].initButtonUL(&tft, 30 + columna * (75+30), 60 + fila * (75 + 10),180, 75,TFT_WHITE, TFT_RED, TFT_WHITE, letras[4], 4);
     columna=4;//se tiene que hacer esto porque sino el calculo de posicion da igual a 5 (columna=2+fila=2*2) y accede a memoria prohibida
     }
     boton[posicion].drawButton();//esta funcion dibuja el boton con los parametros dados por la funcion anterior
    }
  }
  //esta es la parte touch
   for(posicion=0; posicion <=5 ; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==5)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)){
          pastillaElegida=posicion;
          boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
          boton[0,1,2,3].drawButton(false);
          boton[posicion].drawButton(true);
        }
      else boton[posicion].press(false);

    if(boton[4].contains(x,y)){//si se apreta enter alternariva a probar boton[4].justReleased
      *ptr=pastillaElegida;
      verificados[PASTILLA]=true;
      posicion=90;//rompemos el for
    }
             
    //if(boton[posicion].justReleased()) boton[posicion].drawButton();dibuja el boton normal despues de ser pulsado (en esta funcion necesitamos que quede dibujado para marcar que esta pulsado por eso esta comentado)

    if(x<=40 && y<=25) return ATRAS;//si se apretaron las coordenadas que contienen el boton de Atras se termina la funcion  
  }
}

uint8_t menuExistentes(){
 TFT_eSPI_Button boton[2];
 tft.fillScreen(TFT_BLACK);//color del fondo
 //tft.setTextFont(1); sino se especifica usa la predeterminada
 tft.setTextSize(2);
 tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
 tft.drawString("SELECCIONE CUALES MODIFICAR",6,40);
 botonAtras();//mide 40x25px y esta en 0,0
 char letras[2][12] = {"ALARMAS","PROGRAMADAS"};//texto de los botones
 int cord_x=0, cord_y=70, ancho=240, alto=70;//una vez hallamos decidido bien estos parametros usamos las variables en la funcion init directamente para mas comodidad y prolijidad
 unsigned short int x=500,y=500,posicion,botonActual=500;//botonActual, x e y inician en 500 para no activar el boton de atras
 boolean presionado=false,apretoEnter=false;
 
 //parte dibujo
  for(int fila=0; fila<2; fila++){//este for dibuja el menu
     posicion=fila;
     
     boton[posicion].initButtonUL(&tft, cord_x, cord_y + fila * (alto),ancho, alto,TFT_WHITE, TFT_BLUE, TFT_WHITE,letras[posicion], 4);//esta funcion carga los botones desde las esquina superior izq
     //boton[posicion].initButton(&tft, posciion en X ,posicion en Y, ANCHO ,ALTO , color borde,color relleño, color texto,texto, tamaño fuente del texto);
       
       boton[posicion].drawButtonRectangular();//funcion personalizada que dibuja botones rectangulares, se le envia false para boton normal y true para invertido. 
                                                         //aparte se le manda el string que puede ser de mas de 9 caracteres el ultimo parametro es si dibuja el texto en el centro o desde la izq      
        if(posicion==4){
          boton[posicion].initButtonUL(&tft, 0, 250,240, 70,TFT_WHITE, TFT_RED, TFT_WHITE,letras[posicion], 3);//si es el boton de enter letra mas grande y color rojo.
          boton[posicion].drawButtonRectangular(false,true);  // y el texto en el centro                          
        }
  }
  //esta es la parte touch
   for(posicion=0; posicion <=2; posicion++){
       presionado = tft.getTouch(&x,&y);//devuelve true si se apreta la pantalla y guarda las coordenadas en los parametros que le pasamos(tienen que ser unsigned short int si o si porque sino no los toma)
       if(posicion==2)posicion=0;//para que no cuente al infinito reiniciamos posicion, asi siempre esta chequeando los botones
       ////Serial.println(posicion);
        if(presionado && boton[posicion].contains(x,y)){
          botonActual=posicion;
          boton[posicion].press(true);//le decimos al boton que su estado es presionado.sirve para dar pie a otras funciones como justpressed
        }
       
        else boton[posicion].press(false);  


       /* if(boton[posicion].justPressed()){//si se apreto el boton dibujar invertido
          boton[posicion].drawButtonRectangular(true);
         }
       */
        if(boton[posicion].justReleased()) boton[posicion].drawButtonRectangular();//dibuja el boton normal despues de ser pulsado 

        if(x<=40 && y<=25) return ATRAS;//si se apretaron las coordenadas que contienen el boton de Atras se rompe el for

        //if(boton[4].justPressed()) return CONFIRMADO;
     
        switch(botonActual){
          case ALARMA:
          boton[botonActual].drawButtonRectangular(true);
          //delay con timer2 para que la pantalla dibuje el boton
          return ALARMA;
          break;

          case PROGRAMADA:
          boton[botonActual].drawButtonRectangular(true);
          return PROGRAMADA;
          break;
        }
    }  
}

void salvaPantalla(){
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.println("presione la pantalla");
  tft.println("para iniciar");
}

void segundocore(){}

void loop() {

/*if (a==96){
  dibujarCalendario();
  a++;
}*/
switch (MenuActual){
  default:
  MenuActual=menuInicio();
  Serial.println(MenuActual);
  break;
  
  case ALARMA:{//tiene que tener esta llave porque cuando declaras variables dentro de un case gay que usar llaves para determinar su alcance
  //Serial.println("SEXO");
  //Serial.println(PantallaActual);
    int8_t libre=Default;
    for(int posicion=0; posicion<4; posicion++){
      if(!Alarma[posicion].Activa){
        libre=posicion;
        Serial.println(posicion);
        Serial.print(Alarma[posicion].Activa);
        posicion=Default;//esta linea o el break rompen el for
        //break;
      }
    } 
    if (libre==Default){
      tft.fillScreen(TFT_BLACK);
      tft.setTextSize(2);
      tft.setTextColor(TFT_WHITE,TFT_TRANSPARENT);
      tft.drawString("Limite de alarmas alcanzado",20,30);//coordenadas a probar
      PantallaActual=ATRAS;//si intenta entrar al case va a ir a default y caer en config alarma, asi que la mandamos al case ATRAS para que no suceda nada
      MenuActual=Default;//en case de que no entre al case ATRAS ya seteamos menuactual a default
    }
   
    
    switch (PantallaActual){
        
        default:
        PantallaActual=configAlarma();
        Serial.println("898989");
        break;

        /*case HORA:
        tecladoTiempo(Alarma[1].Proxima);
        PantallaActual=Default;
        break;*/

        case DURACION:
        tecladoNumerico(&Alarma[libre].Proxima,DURACION);
        PantallaActual=Default;
        break;

        case INTERVALO:
        tecladoNumerico(&Alarma[libre].Intervalo,INTERVALO);
        PantallaActual=Default;
        break;

        case PASTILLA:
        selector1Pastilla(&Alarma[libre].Pastilla);
        PantallaActual=Default;//la mandamos a 90 para que caiga en default
        break;

        case ATRAS:
        PantallaActual=Default;
        MenuActual=Default;
        break;

        case CONFIRMADO:
        Alarma[libre].Activa=true;
        Alarma[libre].CantidadPastillas = (Alarma[libre].DuracionDias*24)/Alarma[libre].Intervalo;//determinamos la cantidad de pastillas dividiendo los cantidad dias*24 sobre el intervalo
        verificados[1,2,3,4]=false;//reiniciamos los verificadores al falso para la poxima ves que sean usados
//        posicion=0;
        PantallaActual=Default;
        MenuActual=Default;
        break;
      }  
    }
  break;

  case PROGRAMADA:
  
  break;

  case EXISTENTES:
  
    switch (PantallaActual){
  
    default:
    PantallaActual=menuExistentes();
    break;

    case ALARMA:
    /* code */
    break;
    }
  
  break;
  }
  //Serial.println("aaaaa");
}

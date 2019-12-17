#include <Wire.h>
#include "Adafruit_TCS34725.h"

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);

const int dirpin2 = 13;
const int steppin2 = 12;
const int dirpin = 11;
const int steppin = 10;
const int Echo = 9;
const int Trigger = 8;
const int tiempo = 700;
const int led_azul = 3;

int metodo;
int c_blanco = 0;
int c_negro = 0;
int c_azul= 0;
int c_rojo = 0;

void setup(){
  pinMode(Trigger, OUTPUT);
  pinMode(Echo, INPUT);
  digitalWrite(Trigger, LOW);
  pinMode(steppin,OUTPUT); //Motor 1  
  pinMode(dirpin,OUTPUT);
  pinMode(steppin2,OUTPUT); //Motor 2
  pinMode(dirpin2,OUTPUT);  
  pinMode(led_azul, OUTPUT);
  Serial.begin(9600);
}

void infinite_forward(){
  Serial.println("Infinite ");

  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  
  while(true){
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);      //Enviamos un pulso de 10us
    digitalWrite(Trigger, LOW);
    
    t = pulseIn(Echo, HIGH);    //obtenemos el ancho del pulso
    d = t/59;                   //escalamos el tiempo a una distancia en cm
    
    Serial.print("Distancia "); Serial.println(d);

    if(d > 4){
      digitalWrite(dirpin, true);
      digitalWrite(dirpin2, 0); 

      digitalWrite(steppin,HIGH);      
      delayMicroseconds(tiempo);
      digitalWrite(steppin,LOW);
      delayMicroseconds(tiempo);

      digitalWrite(steppin2,HIGH);      
      delayMicroseconds(tiempo);
      digitalWrite(steppin2,LOW);
      delayMicroseconds(tiempo);
    }else{
      break;
    }
  }
}

void forward(int steps){
  Serial.println("Avanzando");
  digitalWrite(dirpin, true);
  digitalWrite(dirpin2, 0);
   
  for(int x = 0; x < steps; x++){  
    digitalWrite(steppin,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin,LOW);
    delayMicroseconds(tiempo);

    digitalWrite(steppin2,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin2,LOW);
    delayMicroseconds(tiempo);
  }
}

void backward(){
  Serial.println("Cambiando direccion ");
  Serial.println("Retrocediendo ");
  digitalWrite(dirpin, 0);
  digitalWrite(dirpin2, true);
  for(int x = 0; x < 200; x++){  
    digitalWrite(steppin,HIGH);    
    delayMicroseconds(tiempo);
    digitalWrite(steppin,LOW);
    delayMicroseconds(tiempo);

    digitalWrite(steppin2,HIGH);    
    delayMicroseconds(tiempo);
    digitalWrite(steppin2,LOW);
    delayMicroseconds(tiempo);  
  }
}

void right(){
  //Wheels counter-clock wise 
  Serial.println("Cambiando direccion");
  Serial.println("Girando a la derecha");
  digitalWrite(dirpin, 0);
  digitalWrite(dirpin2, 0); 

  for(int x = 0; x < 187; x++){  
    digitalWrite(steppin,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin,LOW);
    delayMicroseconds(tiempo);

    digitalWrite(steppin2,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin2,LOW);
    delayMicroseconds(tiempo);
  }
}

void left(){
  //Wheels clockwise
  Serial.println("Cambiando direccion ");
  Serial.println("Girando a la izquierda ");
  digitalWrite(dirpin, true);
  digitalWrite(dirpin2, true); 
  for(int x = 0; x < 187; x++){  
    digitalWrite(steppin,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin,LOW);
    delayMicroseconds(tiempo);

    digitalWrite(steppin2,HIGH);      
    delayMicroseconds(tiempo);
    digitalWrite(steppin2,LOW);
    delayMicroseconds(tiempo);
  }
}

int identify_color(long distance){
  Serial.println("Entré a color");
  long d = distance;
  long rem_dist = d - 1; // Remaining distance for 1 cm  
  bool flag = true;
  int steps;

  if(d != 1){    
    steps = (rem_dist * 200)/20;
    forward(steps);
    Serial.print("steps restantes "); Serial.println(steps);
  }else{
    Serial.print("Objeto Adelante a: ");
    Serial.println(d);

    while(flag){  
      uint16_t r, g, b, c, colorTemp, lux; 
      tcs.getRawData(&r, &g, &b, &c);
      
      if(r > 1000 && g > 1000 && b > 1000){ // Si es blanco gira a la derecha
        Serial.println("Objeto Blanco");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
        if(c_blanco < 3){
          Serial.print("Contador blanco ");
          Serial.print(c_blanco);
          Serial.println(" veces");
          c_blanco++;
        }else{
          metodo = 2;
          c_blanco = 0;  
          flag = false;
        }
      }else if(r < 1000 && g < 1000 && b < 1000){ // Si es negro gira a la izquierda
        if(b > r && b > g){  // Si es azul paro total
          // Insertar código aquí
          Serial.println("Objeto Azul");
          Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
          if(c_azul < 3){
            Serial.println("Contador azul ");
            Serial.print(c_azul);
            Serial.println(" veces");
            c_azul++;
          }else{
            metodo = 4;
            c_azul = 0; 
            flag = false; 
          }
          
        }else{
          Serial.println("Objeto Negro");
          Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
          if(c_negro < 3){
            Serial.println("Contador negro ");
            Serial.print(c_negro);
            Serial.println(" veces");
            c_negro++;
          }else{
            metodo = 1;
            c_negro = 0;  
            flag = false;
          }
        }
      }else if(r > g && r > b){ // Si es rojo agarrar objeto con la pinza
        // Código de pinza
        Serial.println("Objeto Rojo");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
        if(c_rojo < 3){
          Serial.println("Contador rojo ");
          Serial.print(c_rojo);
          Serial.println(" veces");
          c_rojo++;
        }else{
          metodo = 3;
          c_rojo = 0;
          flag = false; 
        }      
      }else if(b > r && b > g){  // Si es azul paro total
          // Insertar código aquí
          Serial.println("Objeto Azul");
          Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
          if(c_azul < 3){
            Serial.println("Contador azul ");
            Serial.print(c_azul);
            Serial.println(" veces");
            c_azul++;
          }else{      
            metodo = 4;  
            c_azul = 0;
            flag = false;
          }
      }else{
        Serial.println("Color no identificado");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
      }
    }
  }
  return metodo;
}

void loop(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);      //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH);    //obtenemos el ancho del pulso
  d = t/59;                   //escalamos el tiempo a una distancia en cm
  
  Serial.print("Distancia "); Serial.println(d);  
  
  if(d < 4){
    infinite_forward();
  }
  
  int metodo = identify_color(d);
  delay(1000);
  

  switch(metodo){
    case 1:
      //girar izquierda
      left();
      break;
    case 2:
      //girar derecha
      right();
      break;
    case 3:
      //agarrar objeto
      Serial.println("Agarrar objeto... Próximamente");
      break;
    case 4:
      //paro total
      digitalWrite(led_azul, HIGH);
      while(true){
        Serial.println("FIN");
      }
      break;
    default:
      Serial.println("Nada");
      break;
  }
  delay(500);
}
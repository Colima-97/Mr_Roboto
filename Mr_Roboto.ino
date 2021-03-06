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

long infinite_forward(){
  Serial.println("Infinite ");

  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  digitalWrite(dirpin, true);
  digitalWrite(dirpin2, 0); 
  
  while(true){
    digitalWrite(Trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);      //Enviamos un pulso de 10us
    digitalWrite(Trigger, LOW);
    
    t = pulseIn(Echo, HIGH);    //obtenemos el ancho del pulso
    d = t/59;                   //escalamos el tiempo a una distancia en cm
    
    Serial.print("Distancia "); Serial.println(d);

    if(d > 4){      
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
  return d;
}

void forward(){ 
  Serial.println("Acercándonos al objeto"); 
  digitalWrite(dirpin, true);
  digitalWrite(dirpin2, 0);

  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros 
  int cont = 0;
  int cont2 = 0;
  
  while(true){
    digitalWrite(Trigger, LOW);
    delayMicroseconds(2);
    digitalWrite(Trigger, HIGH);
    delayMicroseconds(10);      //Enviamos un pulso de 10us
    digitalWrite(Trigger, LOW);
    
    t = pulseIn(Echo, HIGH);    //obtenemos el ancho del pulso
    d = t/59;                   //escalamos el tiempo a una distancia en cm
    
    Serial.print("Distancia "); Serial.println(d);

    if((d >= 2 || d > 1000) && cont < 5){   
      if(d > 1000 ){
        cont++;
      }
      if(cont != 0 && d < 5){
        cont = 0;
      }
      digitalWrite(steppin,HIGH);      
      delayMicroseconds(tiempo);
      digitalWrite(steppin,LOW);
      delayMicroseconds(tiempo);

      digitalWrite(steppin2,HIGH);      
      delayMicroseconds(tiempo);
      digitalWrite(steppin2,LOW);
      delayMicroseconds(tiempo);
      cont2++;     
    }else{
      Serial.println("\n");
      Serial.println(cont2);
      break;
    }      
  }       
}

void backward(int pasos){
  Serial.println("Cambiando direccion ");
  digitalWrite(dirpin, 0);
  digitalWrite(dirpin2, true);
  for(int x = 0; x < pasos; x++){  
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
  Serial.print("Distancia "); Serial.println(d);  
  bool flag = true;  
  metodo = 0;  
    
  if(d > 1 && d < 5){    
    delay(500);
    forward();

    Serial.print("Objeto Adelante a: ");
    Serial.println(d);

    while(flag){  
      uint16_t r, g, b, c, colorTemp, lux; 
      tcs.getRawData(&r, &g, &b, &c);
      
      if(r > 1000 && g > 1000 && b > 1000){ // Si es blanco gira a la derecha
        Serial.println("Objeto Blanco");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
        if(c_blanco < 5){
          Serial.print("Contador blanco ");
          Serial.print(c_blanco);
          Serial.println(" veces");
          c_blanco++;
        }else{
          metodo = 2;
          c_blanco = 0;  
          flag = false;
        }              
      }else if(r < 500 && r < 500 && b < 500){// Color negro
        Serial.println("Objeto Negro");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
        if(c_negro < 5){
          Serial.println("Contador negro ");
          Serial.print(c_negro);
          Serial.println(" veces");
          c_negro++;
        }else{
          metodo = 1;
          c_negro = 0;  
          flag = false;
        } // End of "counter black" else        
      } // End of "black" else
      
      else if(r > g && r > b){ // Si es rojo agarrar objeto con la pinza
        // Código de pinza
        Serial.println("Objeto Rojo");
        Serial.print(r); Serial.print(" "); Serial.print(g); Serial.print(" "); Serial.print(b); Serial.println("");
        if(c_rojo < 5){
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
          if(c_azul < 5){
            Serial.println("Contador azul ");
            Serial.print(c_azul);
            Serial.println(" veces");
            c_azul++;
          }else{      
            metodo = 4;  
            c_azul = 0;
            flag = false;
          }
      }
    } // End of while loop
  } // End of if cycle   
  return metodo;
}

void loop(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros
  int back = 70; // Moves 5 cm back
  long distance = 0;
  
  digitalWrite(Trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);      //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  
  t = pulseIn(Echo, HIGH);    //obtenemos el ancho del pulso
  d = t/59;                   //escalamos el tiempo a una distancia en cm 
  
  if(d > 4){
    distance = infinite_forward();
  }
  if(distance == 0){
    distance = d;    
  }
  int metodo = identify_color(distance);
  delay(1000);
  
  switch(metodo){
    case 1:
      //girar izquierda
      backward(back);
      left();
      break;
    case 2:
      //girar derecha
      backward(back);
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

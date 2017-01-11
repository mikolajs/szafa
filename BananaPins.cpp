#include <wiringPi.h>
#include <mcp23017.h>
#include <stdio.h>

#include "BananaPins.h"

BananaPins::BananaPins(){
  setupWiringPi();
}

void BananaPins::setupWiringPi() {
  wiringPiSetup () ;
  mcp23017Setup (100, 0x20) ;
  mcp23017Setup (200, 0x21) ;
  int i;

  for (i = 0 ; i < 18 ; ++i) {
      pinMode (100 + i, INPUT) ;
      pullUpDnControl (100 + i, PUD_DOWN) ;
      }
  pinMode(115, OUTPUT);
  digitalWrite(115, HIGH);    

  for (i = 0 ; i < 8 ; ++i) {
      pinMode (200 + i, OUTPUT) ;
      digitalWrite(200 + i , LOW);
      }

  }

int BananaPins::isButtonPressed() {
  int pin = 0;
  int colSize = 0;
  for(int j = 0; j < 8; j++) {
      digitalWrite(200 + j, HIGH);
      delay(10);
      if(j < 2 || j > 5) colSize = 13;
      else colSize = 12;
      for (int i = 0 ; i < colSize ; i++) {
        if(digitalRead (100 + i) == 1) {
          //printf ("BananaPins 1 is set on: %i\n", pin + 1) ;
          delay(20);/////!!!!!!!! więcej????
            if(digitalRead (100 + i) == 1){
              printf ("BananaPins is set on: %i\n", pin + 1) ;
              digitalWrite(200 + j, LOW);
              return (pin + 1);
            }
          }
          //printf("J: %d, pin %d\n", j, pin + 1);
          pin++;

      }
      delay(5);
      digitalWrite(200 + j, LOW);
   }
   return -1;
}


void BananaPins::startRoll(){
  digitalWrite(115, HIGH);
}

void BananaPins::stopRoll(){
  digitalWrite(115, LOW);
}

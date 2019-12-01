#include <stdio.h>
#include <wiringPi.h>
 
#define Taster 6		// Pin 22 Orange Pi Zero
#define EncoderA 12	// Pin 24 Orange Pi Zero
#define EncoderB 13	// Pin 26 Orange Pi Zero

int Value=50;
int rechtslauf=0;
int linkslauf=0;
 
void TasterInterrupt (void) 
{ 
printf("Taster\n");
delay (100) ;
}

void EncoderAInterrupt (void) 
{
 rechtslauf=digitalRead(EncoderB);	//  A=0 und B=1 Erste Impuls bei Rechtslauf
//printf("entro la int");
 if(!rechtslauf && linkslauf)		//  A=0 und B=0 Zweite Impuls bei Linkslauf
	{
	Value--;
	printf("Lautstaerke %i \n", Value);
	}
 linkslauf=0;						// Zustand vom Ersten Impuls loeschen 
}
 
void EncoderBInterrupt (void) 
{ 
linkslauf= digitalRead(EncoderA);	//  B=0 und A=1 Erste Impuls bei Linkslauf
if(!linkslauf && rechtslauf)		//  B=0 und A=0 Zweite Impuls bei Rechtslauf
	{
	 Value++;
	 printf("Lautstaerke %i \n", Value);
	}
 rechtslauf=0;					// Zustand vom Ersten Impuls loeschen
}
 
int main (void)
{
  wiringPiSetup () ;
//  pinMode (Taster, INPUT);			// PIN als Ausgang konfigurieren
//  pullUpDnControl (Taster, PUD_UP);		// PIN  mit Pul-Upp versehen
  pinMode (EncoderA, INPUT);			// PIN als Ausgang konfigurieren
  pullUpDnControl (EncoderA, PUD_UP);	// PIN  mit Pul-Upp versehen
  pinMode (EncoderB, INPUT);			// PIN als Ausgang konfigurieren
  pullUpDnControl (EncoderB, PUD_UP);	// PIN  mit Pul-Upp versehen
 
//  wiringPiISR (Taster, INT_EDGE_FALLING, &TasterInterrupt) ;	// Interrupt Routine definieren
  wiringPiISR (EncoderA, INT_EDGE_BOTH, &EncoderAInterrupt);	// Interrupt Routine definieren
  wiringPiISR (EncoderB, INT_EDGE_BOTH, &EncoderBInterrupt);	// Interrupt Routine definieren
  printf("Lautstaerke %i \n", Value);
  while(1) delay (10);								// Endlosschleife
  return 0 ;
}


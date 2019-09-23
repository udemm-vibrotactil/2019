#include <stdio.h>
#include <wiringPi.h>
#include <stdlib.h>
 
void myInterrupt (void) 
{ 
//system("reboot");
system("halt");
}
 
int main (void)
{
  wiringPiSetup () ;
  pinMode (6, INPUT) ;           // PIN 26 als Ausgang konfigurieren
  pullUpDnControl (6, PUD_UP) ;  // PIN 26 mit Pul-Upp versehen
  wiringPiISR (6, INT_EDGE_FALLING, &myInterrupt) ; // Interrupt Routine definieren
  while(1) delay (5000) ;                       // Endlosschleife
  return 0 ;
}


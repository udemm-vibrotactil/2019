#include <stdio.h>
#include "rotaryencoder.h"
#include <wiringPi.h>

int main()
{
printf("Hello!\n");
wiringPiSetup () ;
/*using pins 23/24*/
struct encoder *encoder = setupencoder(12,13);
long value;
while (1)
 {
 updateEncoders();
 long l = encoder->value;
   if(l!=value)
   {
     printf("value: %d\n", (void *)l);
     value = l;
   }
 }
return(0);
}

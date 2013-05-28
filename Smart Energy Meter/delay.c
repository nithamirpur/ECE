//#pragma compact
#include<reg51.h>

// Delay Function Definition

void delay(int a)
{
int i;
for (i=0;i<a;i++);
}

void delay1(int a)
{
int i,j;
for(i=0;i<a;i++)
for(j=0;j<1275;j++);
}
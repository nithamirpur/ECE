//#pragma compact
#include<delay.h>
#include<reg51.h>

// Port Initialization

#define DATAPORT P0	           // Port 2 is utilize for sending command and data
sbit E=P1^0;				  //  Enable pin
sbit RS=P1^1;				  //  Register select
sbit RW=P1^2;				 // Read write
sbit CHK=P0^7;
// Function Definition

void ready(void)
{
RS=0;
RW=1;
E=0;
delay(150);
E=1;
while(CHK==1)
{
E=0;
delay(150);
E=1;
}
}

void writecmd(int z)
{
ready();
RS=0;				   // Register selected for 
RW=0;
DATAPORT=z;
E=1;
delay(150);
E=0;
delay(150);
}

void writedata(char c)
{
ready();
RS=1;
RW=0;
DATAPORT=c;
E=1;
delay(150);
E=0;
delay(150);
}

void string_print(char *s)
{
int i=0,j=0;
while(s[i]!='\0')
{
if(j==15)
{
writecmd(0xC0);
j=0;
}
writedata(s[i]);
writecmd(0x06);
i++;
j++;
}
}

void lcdinit(void)
{
delay(15000);
writecmd(0x30);
delay(4500);
writecmd(0x30);
delay(300);
writecmd(0x30);
delay(650);
writecmd(0x38);
writecmd(0x0c);
writecmd(0x01);
writecmd(0x0E);
}


//#pragma compact
#include<reg51.h>
#include<LCD.h>
#include<keyboard.h>
#include<gsm.h>
#include<delay.h>
#include<stdlib.h>
#include<string.h>


// Data Initialization
char lastkey,send='0';
unsigned int limit,i=0,count=0,reading=0,kwh=2,min=0,hr=0,sec=0,a=0,day=1,month=1,year=2013,track=0,temp=0,b=0,sms=0;
unsigned char MyBuff[15],unit[2],del,k=0,NewMessage=0;
const unsigned char code Response[]="+CMTI";
// Port Initialization

sbit cuty=P1^3;

// Function Decleration
void set_time(void);
void set_limit(void);
void convert(unsigned int count, int p);
void read_notification(void);
void clear_mybuff(void);
void set_date(void);
void creatmsg(void);

// Interrupt Service Routine

//------------------- Interrupt INT0 Service Routine ---------------------------------------------------------

void counter() interrupt 0
{
	count++;                                      // count the pulse comming from the Energy meter
     if(count==1000)
     {
     reading++;																		// Keep track of Kw
     count=0;
     }
 
}

 //----------------------------------- Interrupt TF0 Service Routine------------------------------------------- 
 
 void timer() interrupt 1
 {
	 
   TF0=0;
   TR0=0;
   TH0=0x4B;
   TL0=0xFE;
   TR0=1;
   a++;
   if(a==20)
   {
   sec++;
   a=0;
   if(sec==60)
   {
	 sec=0;
   min++;
	 sms++;
   if(sms==1)
		 send='1';
   if(min==60)
   {
	hr++;
	min=0;
	kwh=kwh+(reading-kwh);
	if(hr==24)
	 { 
		hr=0;
		day++;
   }
   }
   }
   }
   }
//--------------------------------- Interrupt Service  Routine for Tempering------------------------------------------------------------
	 void tempering() interrupt 2
	 {
		 cuty=0;
     temp=1;
	 }
//--------------------------------- Interrupt Service Routine For Serial Communication----------------------------------------------------------

void receive(void) interrupt 4
	 {
		 unsigned char ch;
		 if(RI==1)
		 {
			 RI=0;
			 ch=SBUF;
			 if(ch==Response[k] || (k>4)) // if received character matches or k>4 which means you already have right string
			 {
				 if(ch !=0x0D)
				 {
					MyBuff[k++]=ch;    // data received store data
					}
					else
					{
						NewMessage=1;
						MyBuff[k]='\0';   // data is received
						k=0;              // Reset K 
						}
        }
				else
				{
						if (ch=='P')
						{
							cuty=0;
							clear_mybuff();
						}
						if(ch=='K')
						{
							cuty=1;
							clear_mybuff();
							}
					k=0;  // There is miss match reset counter
				}
		 } 
	 }

//------------------------------- Main Program---------------------------------------------

int main()
{
int j;
TH0=0x4B;
TL0=0xFE;
lcdinit();
set_limit();
//set_date();
set_time();
writecmd(0x01);
TMOD=0x21;
TH0=0x4B;
TL0=0xFE;
IT0=1;
IE=0x87;
init_serial();
string_print("GSM Initializa-tion");
delay1(10);
writecmd(0x01);
gsminit();
string_print("GSM Initializa-tion Done");
delay1(10);
writecmd(0x01);
TR0=1;
writecmd(0x01);
while(1)
{
 if(temp==1)
 {
	 temp=0;
	 send_sms("Meter Is Tempered");
	 writecmd(0x01);
	 string_print("Meter Is tempe-");
	 writecmd(0xC0);
	 string_print("red");
	 b=1;
 }
 if(limit < kwh && track!=1 && b==0)
 {
 send_sms("Limit Crossed");
 track=1;
 }
 if(limit==kwh && track!=1 && b==0)
 {
 send_sms("Limit Reached");
 track=1;
 }
 if( NewMessage==1)
	{
	NewMessage=0;
	read_notification();
	}
	if( b==0)
	{
	if( send=='1')
	{
		send='0';
		unit[0]=kwh+0x30;
		unit[1]='\0';
		send_sms(unit);
		delay(100);
		for(j=0;j<2;j++)
		unit[j]=0;
	}
 string_print("Reading");
 writecmd(0xc0);
 string_print("      ");
 convert(kwh,2);
 string_print("   KWh");
 delay1(300);
 writecmd(0x01);
 string_print("Time");
 writecmd(0xc0);
 string_print("   ");
 convert(hr,2);
 string_print(":");
 convert(min,2);
 string_print(":");
 convert(sec,2);
 delay1(300);
 writecmd(0x01);
 /*string_print("Date");
 writecmd(0xc0);
 string_print("   ");
 convert(day,2);
 string_print("/");
 convert(month,2);
 string_print("/");
 convert(year,4);
 delay1(200);
 writecmd(0x01);*/
 }
}
}

//-------------------------- Set Time Function Definition -------------------------------

void set_time(void)
{ int i=0;
	char key;
  char s[1]={0};
  string_print("Set time");
  delay1(200);
  writecmd(0x01);
	string_print("Enter Seconds");
	writecmd(0xC0);
  while(i<2)
 {
 key=read_switch();
 if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
s[i]=key;
writedata(key);
writecmd(0x06);
if(i==0 && key>='6')
{
	writecmd(0x01);
	string_print(" Value < 6");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Seconds");
	writecmd(0xC0);
	s[i]=0;
	i--;
}
i++;
}
else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<2;i++)
s[i]=0;
i=0;
}
}
sec=atoi(s);
for(i=0;i<2;i++)
s[i]=0;
i=0;
writecmd(0x01);
string_print("Enter Minutes");
writecmd(0xC0);
while(i<2)
{
key=read_switch();
if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
s[i]=key;
writedata(key);
writecmd(0x06);
if(i==0 && key>='6')
{
	writecmd(0x01);
	string_print(" Value < 6");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Minutes");
	writecmd(0xC0);
	s[i]=0;
	i--;
}
i++;
}
else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<2;i++)
s[i]=0;
i=0;
}
}
min=atoi(s);
for(i=0;i<2;i++)
s[i]=0;
i=0;
writecmd(0x01);
string_print("Enter Hours");
writecmd(0xC0);
while(i<2)
{
key=read_switch();
if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
s[i]=key;
writedata(key);
writecmd(0x06);
	if( i==0 && key>='3')
{
	writecmd(0x01);
	string_print(" Value < 3");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Hours");
	writecmd(0xC0);
	s[i]=0;
	i--;
}
if( i==1 && key>='4' && s[i-1]=='2')
{
	writecmd(0x01);
	string_print(" Value < 4");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Hours");
	writecmd(0xC0);
	s[i]=0;
	i--;
	s[i]=0;
	i--;
}
i++;
}
else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<2;i++)
s[i]=0;
i=0;
}
}
hr=atoi(s);
}

// ------------------------Set _limit Function definition-----------------------------------------

void set_limit(void)
{
char key;
char s[4]={0};
int i=0;
string_print("Enter limit");
writecmd(0xc0);
for(;i>=0;i--)
s[i]=0;
i++;
while(i<4)
{
key=read_switch();
if(key=='D')
{
writecmd(0x01);
return;
}
else if(key!='n' && key!='E' && key!='C' && key!='A' && key!='B' && key!='D' && key!='F' )
{
s[i]=key;
i++;
writedata(key);
writecmd(0x06);
}
else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<4;i++)
s[i]=0;
i=0;
}
}
limit=atoi(s);
writecmd(0x01);
}

// ------------------------------Convert function Decleration--------------------------------------

void convert(unsigned int count, int p)
{
int i=0;
if(p==2)
{
char s[2]={0};
while(count>0)
{
s[i]=(count%10);
count=(count/10);
i++;
}
for(i=1;i>=0;i--)
{
writedata(s[i]+0x30);
writecmd(0x06);
}
}
else
{
	char s[4]={0};
while(count>0)
{
s[i]=(count%10);
count=(count/10);
i++;
}
i--;
for(;i>=0;i--)
{
writedata(s[i]+0x30);
writecmd(0x06);
}
}
}



//------------------------------------------------Read Notification-----------------------------------
void read_notification(void)
{
	del=MyBuff[12];
	clear_mybuff();
	gsm_cmd("AT+CMGR=");
	transmit(del);
	gsm_cmd("\r");
	delay1(100);
} 

//--------------------------------------------------Definition of Clear MyBuff--------------------------

void clear_mybuff(void)
{
	int i;
	for(i=0;i<15;i++)
	MyBuff[i]=0;
}

//---------------------------------------------------- Definition of Set date Function --------------------------------

/*void set_date(void)
{
	int i=0;
	char key;
  char s[2]={0};
  char y[5]={0};
  string_print("Set Date");
  delay1(200);
  writecmd(0x01);
	string_print("Enter Day");
	writecmd(0xC0);
  while(i<3)
 {
	 if(i==2)
{
	s[i]='\0';
	i++;
	continue;
}
 key=read_switch();
 if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
s[i]=key;
writedata(key);
writecmd(0x06);
if(i==0 && key>='4' )
{
	writecmd(0x01);
	string_print("0 < Value < 4");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Day");
	writecmd(0xC0);
	s[i]=0;
	i--;
}
if( i==1 && key>='2' && s[i-1]=='3')
{
	writecmd(0x01);
	string_print(" Value < 2");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Day");
	writecmd(0xC0);
	s[i]=0;
	i--;
	s[i]=0;
	i--;
}
if( i==1 && key=='0' && s[i-1]=='0')
{
	writecmd(0x01);
	string_print(" 0 < Value");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Day");
	writecmd(0xC0);
	s[i]=0;
	i--;
	s[i]=0;
	i--;
}
i++;
}

else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<3;i++)
s[i]=0;
i=0;
}
}
day=atoi(s);
for(i=0;i<3;i++)
s[i]=0;
i=0;
writecmd(0x01);
string_print("Enter Month");
writecmd(0xC0);
while(i<3)
{
if(i==2)
{
	s[i]='\0';
	i++;
	continue;
}
key=read_switch();
if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
s[i]=key;
writedata(key);
writecmd(0x06);
if(i==0 && key>='2')
{
	writecmd(0x01);
	string_print(" Value < 2");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Month");
	writecmd(0xC0);
	s[i]=0;
	i--;
}

if( i==1 && key=='0' && s[i-1]=='0')
{
	writecmd(0x01);
	string_print(" 0 < Value");
	delay1(200);
	writecmd(0x01);
	string_print("Enter Month");
	writecmd(0xC0);
	s[i]=0;
	i--;
	s[i]=0;
	i--;
}

i++;
}
else if(key=='C')
{
writecmd(0xC0);
for(i=0;i<3;i++)
s[i]=0;
i=0;
}
}
month=atoi(s);
i=0;
writecmd(0x01);
string_print("Enter Year");
writecmd(0xC0);
while(i<5)
{
	if(i==4)
	{
		s[i]='\0';
		i++;
		continue;
  }
key=read_switch();
if(key!='n' && key!='A' && key!='B' && key!='C' && key!='D' && key!='E' && key!='F')
{
y[i]=key;
writedata(key);
writecmd(0x06);
i++;
}
if(key=='C')
{
writecmd(0xC0);
for(i=0;i<5;i++)
s[i]=0;
i=0;
}
}
year=atoi(y);
writecmd(0x01);
}
*/

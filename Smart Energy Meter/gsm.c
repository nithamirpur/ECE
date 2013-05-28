//#pragma compact
#include<reg51.h>
#include<delay.h>
#include<LCD.h>
void send_sms(char *s);
// Function Definition

void init_serial(void)
{
TH1=0xFD;         // 9600 baud rate
TL1=0xFD;
SCON=0x50;        // Serial mode=1, 8-Bit data, 1 Stop bit, 1 Start bit, Receiving on
TR1=1;
}

void transmit(unsigned char c)
{
SBUF=c;
while(TI!=1);
TI=0;
}

void gsm_cmd(char *s)
{
int i=0;
while(s[i]!='\0')
{
transmit(s[i]);
i++;
}
}

void gsminit(void)
{
send_sms("READY\r");
}

void send_sms(char *s)
{
	gsm_cmd("AT\r");  // AT command to Initialize GSM Modem
	delay(50);
	gsm_cmd("ATe0\r"); // Turn off echo
	delay(50);
	gsm_cmd("AT&W\r"); // Save settings
	delay(50);
	gsm_cmd("AT+CMGF=1\r"); // select text message for sms
	delay1(50);
	gsm_cmd("AT+CNMI=2,1,0,0,0\r"); // notification for new message
	delay(50);
	gsm_cmd("AT+CMGS=\"+919882111750\"\r");
	delay1(50);
	gsm_cmd(s);
	delay1(50);
	SBUF=0x1A;
	delay(50);	
}
#include<reg51.h>
#include<delay.h>

#ifndef GSM
#define GSM
// Function Decleration

void init_serial(void);
void transmit(unsigned char c);
void gsm_cmd(char *s);
void gsminit(void);
void send_sms(char *s);
#endif

//#pragma compact
#include<reg51.h>
#include<delay.h>

// Ports Assignment

sbit RowA = P2^0;     //RowA
sbit RowB = P2^1;     //RowB
sbit RowC = P2^2;     //RowC
sbit RowD = P2^3;     //RowD

sbit C1   = P2^4;     //Column1
sbit C2   = P2^5;     //Column2
sbit C3   = P2^6;     //Column3
sbit C4   = P2^7;     //Column4

char read_switch(void)	
{	
	RowA = 0; RowB = 1; RowC = 1; RowD = 1; 	//Test Row A

	if (C1 == 0) { delay(10000); while (C1==0); return '0'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '1'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '2'; }
	if (C4 == 0) { delay(10000); while (C4==0); return '3'; }

	RowA = 1; RowB = 0; RowC = 1; RowD = 1; 	//Test Row B

	if (C1 == 0) { delay(10000); while (C1==0); return '4'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '5'; }
	if (C3 == 0) { delay(10000); while (C3==0); return '6'; }
	if (C4 == 0) { delay(10000); while (C4==0); return '7'; }
	
	RowA = 1; RowB = 1; RowC = 0; RowD = 1; 	//Test Row C

	if (C1 == 0) { delay(10000); while (C1==0); return '8'; }
	if (C2 == 0) { delay(10000); while (C2==0); return '9'; }
	if (C3 == 0) { delay(10000); while (C3==0); return 'A'; }
	if (C4 == 0) { delay(10000); while (C4==0); return 'B'; }
	
	RowA = 1; RowB = 1; RowC = 1; RowD = 0; 	//Test Row D

	if (C1 == 0) { delay(10000); while (C1==0); return 'C'; }
	if (C2 == 0) { delay(10000); while (C2==0); return 'D'; }
	if (C3 == 0) { delay(10000); while (C3==0); return 'E'; }
	if (C4 == 0) { delay(10000); while (C4==0); return 'F'; }

	return 'n';           	// Means no key has been pressed
}
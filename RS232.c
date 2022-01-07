#include	"rs232.h"

void SerialSetup(void);
void PutStr(register const char *str);
void PutCh(unsigned char c);
void PutStrI(register const char *str);
void PutChI(unsigned char c);

/******************************************************/
/*	Setea el puerto serie.							  */
/******************************************************/
void SerialSetup(void)
{
	BRGH=1;		// data rate for sending.
	SPBRG=cte;	// cte esta definido en el archivo rs232.h.
	SYNC=0;		// asynchronous.
	SPEN=1;		// enable serial port pins.
	CREN=1;		// enable continuous reception.
	SREN=0;		// no effect.
	TXIE=0;		// disable tx interrupts.
	RCIE=1;		// ebable rx interrupts.
	TX9=0;		// 8-bit transmission.
	RX9=0;		// 8-bit reception.
	TXEN=0;		// reset transmitter.
	TXEN=1;		// enable the transmitter.
}

/********************************************************/
/*	Escribe una cadena de caracteres en el puerto serie	*/
/********************************************************/
void PutStr(register const char *str)
{
	while((*str)!=0)
	{
		PutCh(*str);
		str++;
	}
}

/********************************************************/
/*	Escribe un caracter en el puerto serie.				*/
/********************************************************/
void PutCh(unsigned char c)
{
	while(!TXIF)		// set when register is empty
		CLRWDT();
	TXREG=c;
	DelayUs(60);
}
/********************************************************/
/*	Escribe una cadena de caracteres en el puerto serie	*/
/********************************************************/
void PutStrI(register const char *str)
{
	while((*str)!=0)
	{
		PutChI(*str);
		str++;
	}
}
/********************************************************/
/*	Escribe un caracter en el puerto serie.				*/
/********************************************************/
void PutChI(unsigned char c)
{
	while(!TXIF)		// set when register is empty
		CLRWDT();
	TXREG=c;
	DelayUs(60);
}

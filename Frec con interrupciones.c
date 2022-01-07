/********************************************************************/
/*		FRECUENCIMETRO DIGITAL REV. 1.00							*/
/*		PIC16F877A													*/
/*		Checksum final:	0x0f45	(PROTEGIDO)							*/
/*		Checksum final:	0xc680	(ABIERTO)							*/
/*		Compilador: MPLAB IDE 8.00 - HI-TECH 9.60					*/
/*		Fecha de creacion:		17/11/2008							*/
/*		Fecha de modificación:	27/11/2008							*/
/*		Autor:	Mariano Ariel Deville								*/
/********************************************************************/

#define		PIC_CLK			20000000	// Defino la frecuencia del crystal en Hz.
#define		RS232			19200		// Defino la velocidad del puerto serie.
#define		ENTRADA			1
#define		SALIDA			0

#include	"pic.h"
#include	"delay.c"
#include	"lcd.c"
#include	"rs232.c"
#include	"stdio.h"

#if(PIC_CLK>=8000000)					// Autoconfiguro la velocidad del PIC.
	#define	RELOJ HS
#else
	#define RELOJ XT
#endif

__CONFIG(DEBUGDIS & UNPROTECT & LVPDIS & BOREN & PWRTEN & WDTDIS & RELOJ);

unsigned long cont,cont1;
unsigned char cadena[9];

void interrupt isr(void);

void main(void)
{ 
	TRISA0=ENTRADA;		// 
	TRISA1=SALIDA;		// 
	TRISA2=ENTRADA;		// 
	TRISA3=ENTRADA;		// 
	TRISA4=ENTRADA;		// 
	TRISA5=ENTRADA;		// 

	TRISB0=ENTRADA;		// 
	TRISB1=ENTRADA;		// 
	TRISB2=SALIDA;	   	// Salida para el LCD RS.
	TRISB3=SALIDA;   	// Salida para el LCD E.
	TRISB4=SALIDA; 	  	// Salida para el LCD AD4.
	TRISB5=SALIDA;   	// Salida para el LCD AD5.
	TRISB6=SALIDA;   	// Salida para el LCD AD6.
	TRISB7=SALIDA;   	// Salida para el LCD AD7.
 
	TRISC0=ENTRADA;		// 
	TRISC1=ENTRADA;		// 
	TRISC2=ENTRADA;		// 
	TRISC3=ENTRADA;		// 
	TRISC4=ENTRADA;		// 
	TRISC5=ENTRADA;		// 
	TRISC6=SALIDA;		// Salida TX.
	TRISC7=ENTRADA;		// Entrada RX.
 
	TRISD0=ENTRADA;		// 
	TRISD1=ENTRADA;		// 
	TRISD2=ENTRADA;		// 
	TRISD3=ENTRADA;		// 
	TRISD4=ENTRADA;		// 
	TRISD5=ENTRADA;		// 
	TRISD6=ENTRADA;		// 
	TRISD7=ENTRADA;		// 

	TRISE0=ENTRADA;		//
	TRISE1=ENTRADA;		//
	TRISE2=ENTRADA;		//

	GIE=0;
	INTEDG=1; 			// Interrupcion en el flanco ascendente de RB0
	RBPU=1;				// RB pull-ups estan deshabilitadas

	PEIE=1;				// Interrupcion externa habilitada
	INTE=1;				// Interrupcion RB0/INT habilitada
	INTF=0;				// Borro la bandera de la interrupcion.
	SerialSetup();		// Inicializo el puerto serie.
    Lcd_Init();			// Inicializa LCD.
    Lcd_Clear();		// y borra la pantalla.
	Lcd_Puts("FRECUENCIMETRO");
	Lcd_Goto(0x40);
	Lcd_Puts("DIGITAL");
	PutStr("FRECUENCIMETRO DIGITAL");
	PutCh(13);							// Retorno de carro.
	for(;;)
	{
		cont=0;
		GIE=1;
		DelayMs(2);
		DelayUs(40);
		GIE=0;
		sprintf(cadena," %u",cont); 
		PutStr(cadena);					// Lo mando por RS232.
		PutCh(13);						// Retorno de carro.
	}
}

void interrupt isr(void)
{
	CLRWDT();
	if(INTF&&INTE)
	{
		cont++;
		INTF=0;
	}
}


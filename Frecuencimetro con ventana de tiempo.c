/********************************************************************/
/*		FRECUENCIMETRO DIGITAL REV. 1.00							*/
/*		PIC16F877A													*/
/*		Checksum final:	0x0f45	(PROTEGIDO)							*/
/*		Checksum final:	0xc680	(ABIERTO)							*/
/*		Compilador: MPLAB IDE 8.00 - HI-TECH 9.60					*/
/*		Fecha de creacion:		17/11/2008							*/
/*		Fecha de modificación:	02/12/2008							*/
/*		Autor:	Mariano Ariel Deville								*/
/********************************************************************/
/*	Utilizo el TMR1 para generar una ventana de tiempo en la cual	*/
/*		cuento la cantidad de pulsos en el pin RB0.					*/
/********************************************************************/

#define		PIC_CLK		20000000	// Defino la frecuencia del crystal en Hz.
#define		RS232		19200		// Defino la velocidad del puerto serie.
#define		ENTRADA		1
#define		SALIDA		0

#include	"pic.h"
#include	"delay.c"
#include	"lcd.c"
#include	"rs232.c"
#include	"adc.c"
#include	"stdio.h"
#include	"utiles.c"

#if(PIC_CLK>=8000000)				// Autoconfiguro la velocidad del PIC.
	#define	RELOJ HS
#else
	#define RELOJ XT
#endif

__CONFIG(DEBUGDIS & UNPROTECT & LVPDIS & BOREN & PWRTEN & WDTEN & RELOJ);

unsigned long frecuencia,cont;
unsigned char valor[10];
bit bandera;

unsigned long Medicion(void);
void interrupt isr(void);

void main(void)
{ 
	TRISA0=ENTRADA;		// 
	TRISA1=ENTRADA;		// 
	TRISA2=ENTRADA;		// 
	TRISA3=ENTRADA;		// 
	TRISA4=ENTRADA;		// 
	TRISA5=ENTRADA;		// 

	TRISB0=ENTRADA;		// Entrada de señal a medir.
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

	PEIE=0;				// Interrupcion externa inhabilitada.
	INTEDG=0; 			// Interrupcion en el flanco descendente de RB0.
	RBPU=1;				// RB pull-ups estan deshabilitadas.
	GIE=1;				// Interrupciones globales deshabilitadas.
	T1CKPS0=1; 			// Preescaler a 1/8.
	T1CKPS1=1; 			//
	T1OSCEN=1;			// Oscilador del TMR1 ENABLED.(Disparo el contador)
	TMR1CS=0;  			// Reloj interno (Fosc/4).


	SerialSetup();		// Inicializo el puerto serie.
    Lcd_Init();			// Inicializa LCD.
    Lcd_Clear();		// Borra la pantalla.

	frecuencia=0;
	Lcd_Puts("FRECUENCIMETRO");
	PutStr("FRECUENCIMETRO DIGITAL");
	PutCh(13);						// Retorno de carro.
	for(;;)
	{
		Lcd_Goto(0x40);
		Lcd_Puts("F: ");
		PutStr("F: ");
		frecuencia=Medicion();		// Comienzo la medición.
		sprintf(valor,"%u",frecuencia);
		Lcd_Puts(valor);
		PutStr(valor);				// Lo mando por RS232.
		Lcd_Puts(" Hz.");
		PutStr(" Hz.");				// Agrego la unidad.
		PutCh(13);					// Retorno de carro.
	}
}
unsigned long Medicion(void)
{
	unsigned long calculo;
	bandera=0;
	cont=0;							// Inicializo el contador.
	INTE=1;							// Interrupcion RB0/INT habilitada.
	INTF=0;							// Borro la bandera de la interrupcion externa.
	TMR1IF=0;						// Borro la bandera de interrupcion por TMR1.
	TMR1IE=1;						// Interrupcion por TMR1 habilitada.
	PEIE=1;							// Interrupcion externa habilitada.
	TMR1ON=1;						// Enciendo TMR1.
	while(!bandera) CLRWDT();		// Espero que termine la venta de tiempo.
	PEIE=0;							// Deshabilito la interrupcion externa.
	cont=(unsigned int)(cont*1.07);	// Corrección en la medicion.

	// tiempo TMR1=((65536-precarga)*Preescaler)*tiempo instruccion
	// tiempo instruccion = 4 / frecuencia osc.
	// Para preescaler de 1:8 el calculo de TMR1 es igual a .1048576

	calculo=(unsigned long)(cont/0.1048576);	// En base a la ventana y el contador calculo la frecuencia.
	return(calculo);
}
void interrupt isr(void)
{
	if(INTE && INTF)			// Interrupción por RB0.
	{
		cont++;				// Cuento los pulsos de entrada.
		INTF=0;				// Borro la bandera de INT.
		return;
	}
	if(TMR1IE && TMR1IF)			// Interrupción por TMR1.
	{
		bandera=1;
		TMR1IF=0;			// Borro la bandera del TMR1.
		return;
	}
}

/********************************************************************/
/*		FRECUENCIMETRO DIGITAL REV. 1.00							*/
/*		PIC16F877A													*/
/*		Checksum final:	0x0f45	(PROTEGIDO)							*/
/*		Checksum final:	0xc680	(ABIERTO)							*/
/*		Compilador: MPLAB IDE 8.00 - HI-TECH 9.60					*/
/*		Fecha de creacion:		02/11/2008							*/
/*		Fecha de modificación:	04/12/2008							*/
/*		Autor:	Mariano Ariel Deville								*/
/********************************************************************/
/*	El flanco ascendente de la señal de entrada dispara el contador	*/
/*	TMR1 y con el proximo flanco lo detiene, en base al contador	*/
/*	TMR1 y el tiempo que tarda en incrementar calculo la frecuencia	*/
/*	de la señal de entrada.											*/
/********************************************************************/

#define		PIC_CLK		20000000	// Defino la frecuencia del crystal en Hz.
#define		RS232		19200		// Defino la velocidad del puerto serie.
#define		ENTRADA		1
#define		SALIDA		0
#define		T_CONT		0.0000008	//	Tiempo que tarda en incrementarse TMR1.

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

unsigned long frecuencia;
unsigned char valor[10];
bit error;

unsigned long Medicion(void);
void interrupt isr(void);

void main(void)
{ 
	TRISA=ENTRADA;		// 

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
 
	TRISD=ENTRADA;		// 
	TRISE=ENTRADA;		//

	PEIE=0;				// Interrupcion externa inhabilitada.
	INTEDG=1; 			// Interrupcion en el flanco ascendente de RB0.
	RBPU=1;				// RB pull-ups estan deshabilitadas.
	GIE=1;				// Interrupciones globales habilitadas.
	INTE=1;				// Interrupcion RB0/INT habilitada.
	TMR1IE=1;			// Interrupcion desborde TMR1 habilitada.
	T1CKPS0=0; 			// Preescaler a 1/4.
	T1CKPS1=1; 			//
	TMR1CS=0;  			// Reloj interno (Fosc/4) para TMR1.

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
		DelayMs(50);
	}
}

unsigned long Medicion(void)
{
	unsigned long calculo;
	unsigned int aux=0;
	error=0;
	T1OSCEN=0;						// Oscilador de TMR1 apagado.
	TMR1H=0;						// Reseteo TMR1.
	TMR1L=0;						// 
	INTF=0;							// Borro la bandera de la interrupcion externa.
	TMR1IF=0;						// Borro la bandera de la interrupción TMR1.
	PEIE=1;							// Interrupcion externa habilitada.
	while(!TMR1ON && aux<=65000)	// Espero que comience la medición.
	{
		CLRWDT();
		aux++;						// Salida en caso de no haber señal.
	}
	while(TMR1ON)					// Espero que termine la medición.
		CLRWDT();
	PEIE=0;							// Deshabilito la interrupcion externa.
	calculo=TMR1H;					// Cargo la parte alta del contador TMR1
	calculo=calculo<<8;				// lo desplazo ocho lugares
	calculo|=TMR1L;					// y le agrego la parte baja del contador TMR1.
// Para preescaler de 1:4 el calculo de TMR1 es igual a .0524288
// cada ciclo del TMR1 tarda en completarse 0.8 Us
	calculo=(unsigned long)(1/(calculo*T_CONT));
	if(aux>65000||error)
		calculo=0;					// La medición no se concreto.
	return(calculo);
}

void interrupt isr(void)
{
	if(INTE && INTF)				// Interrupción por RB0.
	{
		TMR1ON=!TMR1ON;				// Enciendo/apago TMR1.
		INTF=0;						// Borro la bandera de INT.
		return;
	}
	if(TMR1IE && TMR1IF)			// Interrupción por TMR1.
	{
		error=1;					// Aviso que la lectura no es valida
		TMR1ON=0;					// porque se desbordo TMR1.
		TMR1IF=0;
		return;
	}
}

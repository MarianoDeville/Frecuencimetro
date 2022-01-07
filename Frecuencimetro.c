/********************************************************************/
/*		FRECUENCIMETRO DIGITAL REV. 1.00							*/
/*		PIC16F877A													*/
/*		Checksum final:	0x0f45	(PROTEGIDO)							*/
/*		Checksum final:	0xc680	(ABIERTO)							*/
/*		Compilador: MPLAB IDE 8.00 - HI-TECH 9.60					*/
/*		Fecha de creacion:		04/12/2008							*/
/*		Fecha de modificación:	22/12/2008							*/
/*		Autor:	Mariano Ariel Deville								*/
/********************************************************************/
/*		Utilizo TMR0 para generar una ventana de tiempo en la cual 	*/
/*	disparo TMR1 y en base a la cantidad de pulsos contados y el	*/
/*	tiempo de la ventana puedo calcular la frecuencia.				*/
/*		Para auto configurar la ventana hago una primera medición y */
/*	calculo el rango en el que se va a medir la frecuencia.			*/
/*		Es posible tambien entrar en un modo de rango manual.		*/
/********************************************************************/

#define		PIC_CLK		4000000		// Defino la frecuencia del crystal en Hz.
#define		RS232		19200		// Defino la velocidad del puerto serie.
#define		DERECHO		RB0			// Botón de navegación.
#define		IZQUIERDO	RB1			// Botón de navegación.
#define		ENTRADA		1
#define		SALIDA		0

#include	"pic.h"
#include	"delay.c"
#include	"lcd.c"
#include	"rs232.c"
#include	"adc.c"
#include	"utiles.c"

/************************************************************************************************/
/*				Autoconfiguración de los parametros según la velocidad del cristal				*/
#if(PIC_CLK>=4000000)				// Autoconfiguro la velocidad del PIC.
	#define T_CONT		0.016384	// Tiempo hasta desbordar un ciclo de TMR0 con CLK 4 MHz.
	#define CONF_PRE	0			// Configuración del PREESCALER de TMR0 para CLK 4 MHz.
#endif

#if(PIC_CLK>=20000000)				// Autoconfiguro la velocidad del PIC.
	#define T_CONT		0.0131072	// Tiempo hasta desbordar un ciclo de TMR0 con CLK 20 MHz.
	#define CONF_PRE	1			// Configuración del PREESCALER de TMR0 para CLK 20 MHz.
#endif

#if(PIC_CLK>=4000000)				// Autoconfiguro la velocidad del PIC.
	#define	RELOJ HS
#else
	#define RELOJ XT
#endif
/************************************************************************************************/

__CONFIG(DEBUGDIS & UNPROTECT & LVPDIS & BOREN & PWRTEN & WDTEN & RELOJ);

bank1 unsigned long frecuencia,rango,almacenado;
bank1 unsigned char valor[9],vueltas,ventana;
bank1 bit error,rango_manual,menu_rango;

unsigned long Medicion(void);
unsigned char AutoRango(void);
void interrupt isr(void);
void Visualizacion(void);

void main(void)
{ 
	ADCON1=0b00000110;				// Confiura el PORTA como todo digital.

	TRISA=ENTRADA;					// 

	TRISB0=ENTRADA;					// Teclado.
	TRISB1=ENTRADA;					// Teclado.
	TRISB2=SALIDA;	 		  		// Salida para el LCD RS.
	TRISB3=SALIDA;   				// Salida para el LCD E.
	TRISB4=SALIDA; 	 			 	// Salida para el LCD AD4.
	TRISB5=SALIDA;   				// Salida para el LCD AD5.
	TRISB6=SALIDA;   				// Salida para el LCD AD6.
	TRISB7=SALIDA;   				// Salida para el LCD AD7.
 
	TRISC0=ENTRADA;					// Entrada de señal a medir.
	TRISC1=ENTRADA;					// 
	TRISC2=ENTRADA;					// 
	TRISC3=ENTRADA;					// 
	TRISC4=ENTRADA;					// 
	TRISC5=ENTRADA;					// 
	TRISC6=SALIDA;					// Salida TX.
	TRISC7=ENTRADA;					// Entrada RX.
 
	TRISD=ENTRADA;					// 
	TRISE=ENTRADA;					//
/****************************************************************************/
/*			Configuración de las interrupciones								*/
	PEIE=1;							// Interrupcion externa habilitada.
	RBPU=1;							// RB pull-ups estan deshabilitadas.
	GIE=1;							// Interrupciones globales habilitadas.
	INTE=1;							// Interrupcion RB0/INT habilitada.
	TMR1IE=1;						// Interrupcion desborde TMR1 deshabilitada.
	TMR0IE=0;						// Interrupción por TMR0 deshabilitada.
	RCIE=0;							// Interrupcion recepcion USART deshabilitada.
/****************************************************************************/
/*			Configuración del Timer 0										*/
	T0CS=0;							// Oscilador interno.
	T0SE=0;							// Flanco ascendente.
	PSA=0;							// Asigno el preescaler a TMR0.
	PS0=1;							// Configuro el Preescaler.
	PS1=CONF_PRE;					// 
	PS2=1;							// 
/****************************************************************************/
/*			Configuración del Timer 1										*/
	T1CKPS0=0; 						// Preescaler TMR1 a 1.
	T1CKPS1=0; 						//
	T1SYNC=0;						// Sincronizo con clock externo.
	TMR1CS=1;  						// Tomo la señal en RC0.
/****************************************************************************/
/*			Inicializo display y comunicación RS232							*/
	SerialSetup();					// Inicializo el puerto serie.
    Lcd_Init();						// Inicializa LCD.
    Lcd_Clear();					// Borra la pantalla.
	frecuencia=0;
	Imprimir_Lcd("FRECUENCIMETRO","DIGITAL",1);
	PutStr("FRECUENCIMETRO DIGITAL");
	PutCh(13);						// Retorno de carro.
	DelayMs(250);
	for(;;)
	{
		frecuencia=Medicion();		// Comienzo la medición.
		if(almacenado!=frecuencia)	// Solo muestra si hay cambio.
		{
			almacenado=frecuencia;	// Actualizo la referencia para la proxima vuelta.
			Visualizacion();		// Visualizo los resultados.
		}
	}
}
/************************************************************/
/*		Función que mide y calcula la frecuencia.	 		*/
/************************************************************/
unsigned long Medicion(void)
{
	unsigned long calculo;
	if(!rango_manual)
		ventana=AutoRango();		// Defino la escala de medicón.
	vueltas=0;
	error=0;
	TMR1H=0;						// Reseteo TMR1.
	TMR1L=0;						// 
	TMR1IF=0;						// Borro la bandera de la interrupción TMR1.
	TMR0IF=0;						// Borro la bandera de la interrupción TMR0.
	TMR1ON=0;						// TMR1 habilitado.
	TMR0=230;						// Dejo el TMR0 cerca del desborde.
	TMR0IE=1;						// Interrupción por TMR0 habilitada.
	while(!TMR1ON)					// Espero que comience la medición.
		CLRWDT();
	while(TMR1ON)					// Espero que termine la medición.
		CLRWDT();
	TMR0IE=0;						// Apago TMR0.
	calculo=TMR1H;					// Cargo la parte alta del contador TMR1
	calculo=calculo<<8;				// lo desplazo ocho lugares
	calculo|=TMR1L;					// y le agrego la parte baja del contador TMR1.
	calculo=(unsigned long)(calculo/(T_CONT*ventana));
	if(error)
		return 0;					// La medición no se concreto.
	return(calculo);
}
/************************************************************/
/*		Función auto rango, demanda 120 ms de medición 		*/
/************************************************************/
unsigned char AutoRango(void)
{
	unsigned long calculo;
	error=0;
	ventana=4;						// Defino la ventana de medicion.
	vueltas=0;
	TMR1H=0;						// Reseteo TMR1.
	TMR1L=0;						// 
	TMR1IF=0;						// Borro la bandera de la interrupción TMR1.
	TMR0IF=0;						// Borro la bandera de la interrupción TMR0.
	TMR1ON=0;						// TMR1 habilitado.
	TMR0=230;						// Dejo el TMR0 cerca del desborde.
	TMR0IE=1;						// Interrupción por TMR0 habilitada.
	while(!TMR1ON)					// Espero que comience la medición.
		CLRWDT();
	while(TMR1ON)					// Espero que termine la medición.
		CLRWDT();
	TMR0IE=0;						// Apago TMR0.
	calculo=TMR1H;					// Cargo la parte alta del contador TMR1
	calculo=calculo<<8;				// lo desplazo ocho lugares
	calculo|=TMR1L;					// y le agrego la parte baja del contador TMR1.
	if(error)						// Uso la escala mas baja para medir, mas de 1MHz.
		return 3;
	if(calculo<1049)				// Escala hasta 16 KHz.
		return 250;
	if(calculo<1311)				// Escala hasta 20 KHz.
		return 200;
	if(calculo<1638)				// Escala hasta 25 KHz.
		return 160;
	if(calculo<2097)				// Escala hasta 32 KHz.
		return 125;
	if(calculo<2621)				// Escala hasta 40 KHz.
		return 100;
	if(calculo<3277)				// Escala hasta 50 KHz.
		return 80;
	if(calculo<5243)				// Escala hasta 80 KHz.
		return 50;
	if(calculo<6554)				// Escala hasta 100 KHz.
		return 40;
	if(calculo<10486)				// Escala hasta 160 KHz.
		return 25;
	if(calculo<13107)				// Escala hasta 200 KHz.
		return 20;
	if(calculo<26214)				// Escala hasta 400 KHz.
		return 10;
	if(calculo<52429)				// Escala hasta 800 KHz.
		return 5;
	return 4;						// Escala hasta 1 MHz.
}
/************************************************************/
/*				Corazón de la medición				 		*/
/************************************************************/
void interrupt isr(void)
{
	if(TMR0IE && TMR0IF)			// Interrupción por RB0.
	{
		if(!vueltas||vueltas>=ventana)
			TMR1ON=!TMR1ON;			// Enciendo/Apago TMR1.
		vueltas++;
		TMR0IF=0;					// Bajo la bandera de la interrupción.
		return;
	}
	if(TMR1IE && TMR1IF)			// Interrupción por TMR1.
	{
		error=1;					// Aviso que la lectura no es valida
		TMR1ON=0;					// Apago porque se desbordo TMR1.
		TMR1IF=0;					// Bajo la bandera de la interrupción.
		return;
	}
	if(INTE && INTF)				// Interrupción por teclado.
	{
		TMR1ON=0;					// Apago TMR1 para entrar en el menu.
		TMR0IE=0;					// Apago TMR0 para entrar en el menu.
		rango_manual=1;				// Bandera para el rango manual.
		while(DERECHO)
		{
			if(IZQUIERDO)			// Salgo del modo manual.
				rango_manual=0;
			CLRWDT();
		}
		INTF=0;						// Bajo la bandera de la interrupción.
		if(ventana<10)
		{
			ventana++;
			return;
		}
		if(ventana)
		{
			ventana+=2;
			return;
		}
		if(ventana<50)
		{
			ventana+=4;
			return;
		}
		if(ventana<80)
		{
			ventana+=6;
			return;
		}
		if(ventana<160)
		{
			ventana+=10;
			return;
		}
		if(ventana>=130)
		{
			ventana+=40;
			if(ventana>250)
				ventana=0;
			return;
		}
	}
}
/************************************************************/
/*		Muestro en display y envio por RS232 la medición.	*/
/************************************************************/
void Visualizacion(void)
{
	rango=(unsigned long)(32768/(T_CONT*ventana));
	rango=(rango/1000);					// Lo expreso en KHz.
	IntToStr(rango,valor);				// Convierto de long a string.
	PutStr("Rango de la escala: ");
	PutStr(valor);
	PutStr(" a ");						// Separador
	Lcd_Clear();
	Lcd_Goto(0x0);
	Lcd_Puts("Escala: ");
	rango=(unsigned long)(65536/(T_CONT*ventana));
	rango=(rango/1000);					// Lo expreso en KHz.
	IntToStr(rango,valor);				// Convierto de long a string.
	Lcd_Puts(valor);
	Lcd_Puts("KHz.");
	PutStr(valor);
	PutStr(" KHz.");
	if(error)
	{
		Imprimir_Lcd("","Fuera de escala.",0);
		PutStr(" Fuera de escala.");
		PutCh(13);						// Retorno de carro.
		return;
	}
	IntToStr(frecuencia,valor);			// Convierto de long a string.
	Imprimir_Lcd("","F: ",0);
	Lcd_Puts(valor);
	Lcd_Puts(" Hz.");
	PutStr(" Frecuencia: ");
	PutStr(valor);						// Lo mando por RS232.
	PutStr(" Hz.");						// Agrego la unidad.
	PutCh(13);							// Retorno de carro.
	return;
}

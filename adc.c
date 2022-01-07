/****************************************************************/
/*	Configuro los puertos a utilizar							*/
/****************************************************************/
void InitialiseADC (unsigned char ADC_Channel)
{
	if(ADC_Channel==0)		TRISA0=1;
	else if(ADC_Channel==1)	TRISA1=1;
	else if(ADC_Channel==2)	TRISA2=1;
	else if(ADC_Channel==3)	TRISA3=1;

//	ADCON1=0b10000100;		/* Analogue-RA0/RA1/RA3 Digital-RA2/RA5	*/
/*
	PCFG	AN7	AN6	AN5	AN4	AN3	AN2	AN1	AN0
	0000	 A	 A	 A	 A	 A	 A	 A	 A
	0001	 A	 A	 A	 A	 +	 A	 A	 A
	0010	 D	 D	 D	 A	 A	 A	 A	 A
	0011	 D	 D	 D	 A	 +	 A	 A	 A
	0100	 D	 D	 D	 D	 A	 D	 A	 A
	0101	 D	 D	 D	 D	 +	 D	 A	 A
	011x	 D	 D	 D	 D	 D	 D	 D	 D
	1000	 A	 A	 A	 A	 +	 -	 A	 A
	1001	 D	 D	 A	 A	 A	 A	 A	 A
	1010	 D	 D	 A	 A	 +	 A	 A	 A
	1011	 D	 D	 A	 A	 +	 -	 A	 A
	1100	 D	 D	 D	 A	 +	 -	 A	 A
	1101	 D	 D	 D	 D	 +	 -	 A	 A
	1110	 D	 D	 D	 D	 D	 D	 D	 A
	1111	 D	 D	 D	 D	 +	 -	 D	 A

*/
} 

/****************************************************************/
/*	Conversion Analogica / Digital								*/
/****************************************************************/
unsigned int ReadADC(unsigned char ADC_Channel)
{
	volatile unsigned int ADC_VALUE;
  	//ADCON0=(ADC_Channel << 3) + 65;		// Channel + ADON + Fosc/8.
  	ADCON0=(ADC_Channel << 3) + 1;		// Channel + ADON + Fosc/2.
  	ADIF=0;							 	// Borra la bandera de interrupcion del AD.					
	ADRESL=0; 							// Borra ADRESL.
	ADRESH=0; 							// Borra ADRESH.
  	//GODONE=1;								// Comienza el proceso de conversion PIC 16F777.
  	ADGO=1;								// Comienza el proceso de conversion PIC 16F877.
  	CLRWDT();						    // Borra el WDT.
  	while(!ADIF)continue;				// Espera el fin de la conversion.
	ADC_VALUE=ADRESL;					// Obtiene la parte alta de CCP1.
	ADC_VALUE+=(ADRESH << 8);	  		// Obtiene la parte baja de CCP1.
	return(ADC_VALUE);    				// Retorna el valor de la conversion de 10 bits.
}

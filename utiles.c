void IntToStr(unsigned long origen,unsigned char *destino);

/************************************************************/
/*	Convierto numero esteros a cadena de caracteres			*/
/*	lo puedo utilizar con numeros de hasta cuatro digitos.	*/
/************************************************************/
void IntToStr(unsigned long origen,unsigned char *destino)
{
	volatile unsigned long aux1,aux2,aux3,aux4,aux5,aux6,aux7;	// Variables auxiliares para la conversion.
	CLRWDT();
	aux1=(origen/1000000);
	if(aux1)
	{
		(*destino)=aux1+48;
		destino++;
	}
	aux2=(origen-aux1*1000000)/100000;
	if(aux2||aux1)
	{
		(*destino)=aux2+48;
		destino++;
	}
	aux3=(origen-aux1*1000000-aux2*100000)/10000;
	if(aux3||aux2||aux1)
	{
		(*destino)=aux3+48;
		destino++;
	}
	aux4=(origen-aux1*1000000-aux2*100000-aux3*10000)/1000;
	if(aux4||aux3||aux2||aux1)
	{
		(*destino)=aux4+48;
		destino++;
	}
	aux5=(origen-aux1*1000000-aux2*100000-aux3*10000-aux4*1000)/100;
	if(aux5||aux4||aux3||aux2||aux1)
	{
		(*destino)=aux5+48;
		destino++;
	}
	aux6=(origen-aux1*1000000-aux2*100000-aux3*10000-aux4*1000-aux5*100)/10;
	if(aux6||aux5||aux4||aux3||aux2||aux1)
	{
		(*destino)=aux6+48;
		destino++;
	}
	aux7=(origen-aux1*1000000-aux2*100000-aux3*10000-aux4*1000-aux5*100-aux6*10);
	(*destino)=aux7+48;
	destino++;
	(*destino)=0;
}


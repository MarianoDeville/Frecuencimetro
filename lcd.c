/* write a byte to the LCD in 4 bit mode */

static bit LCD_RS	@ ((unsigned)&PORTB*8+2);	// Register select
static bit LCD_EN	@ ((unsigned)&PORTB*8+3);	// Enable

void Lcd_Write(unsigned char);
void Lcd_Clear(void);
void Lcd_Puts(const char * s);
void Lcd_Goto(unsigned char pos);
void Lcd_Init(void);
void Lcd_Putch(char c);
void ScrollMessage(unsigned char row,const char Message[]);
void Imprimir_Lcd(const char *LINEA_1,const char *LINEA_2,unsigned char BORRAR);

#define	LCD_STROBE	((LCD_EN = 1),(LCD_EN=0))

//#pragma interrupt_level 1
void Lcd_Write(unsigned char c)
{
	PORTB &=0x0F;
	PORTB |=c&0xF0;
	LCD_STROBE;
	PORTB &=0x0F;
	PORTB |=(c << 4)&0xF0;
	LCD_STROBE;
	DelayMs(1);
}

//#pragma interrupt_level 1
void Lcd_Clear(void)
{
	LCD_RS = 0;
	Lcd_Write(0x1);
}

//#pragma interrupt_level 1
void Lcd_Puts(const char * s)
{
	LCD_RS = 1;	// write characters
	while(*s)
	Lcd_Write(*s++);
}

//#pragma interrupt_level 1
void Lcd_Goto(unsigned char pos)
{
	LCD_RS = 0;
	Lcd_Write(0x80+pos);
}

// 		initialise the LCD - put into 4 bit mode 
//		movlw		b'00001111'		;LCD on, Cursor On, Parpadeo On
//		movlw		b'00001110'		;LCD on, Cursor On, Parpadeo Off
//		movlw		b'00001101'		;LCD on, Cursor Off, Parpadeo On
//		movlw		b'00001100'		;LCD on, cursor off. Parpadeo OFF
void Lcd_Init(void)
{
	LCD_RS = 0;				// write control bytes
	DelayMs(15);			// power on 	delay
	PORTB &=0x0F;			// -- Clear the port
	PORTB |= 0B00110000;	// Es para que salga el dato por RB7,RB6,RB5,RB4
	LCD_STROBE;
	LCD_STROBE;
	DelayMs(1);
	PORTB &=0x0F;			// -- Clear the port
	PORTB |= 0B00100000;
	LCD_STROBE;
	DelayMs(1);
	Lcd_Write(0x28);		// 4 bit mode, 1/16 duty, 5x8 font
	Lcd_Write(0b00001100);	// configuro la visualización.
}

// #pragma interrupt_level 1
void Lcd_Putch(char c)
{
	char str[2];
	str[0]=c;
	str[1]=0;
	Lcd_Puts(str);
}

//#pragma interrupt_level 1
void ScrollMessage(unsigned char row,const char Message[])
{
	char TempS[30];
	unsigned int  MHead=0,Done=0,count;
	if(row >1) row=1;
	row=row*40;
	while(Done==0)
	{
 		for(count=0;count<16;count++)
		{
			TempS[count]=Message[MHead+count];
			if(Message[MHead+count+1]==0) Done=1;
		}
		MHead++;
		Lcd_Goto(row);
		Lcd_Puts(TempS);
		DelayMs(60);
	}
}

//#pragma interrupt_level 1
void Imprimir_Lcd(const char *LINEA_1,const char *LINEA_2,unsigned char BORRAR)
{
	if(BORRAR)	Lcd_Clear();
	Lcd_Goto(0x00);
	Lcd_Puts(LINEA_1);
	Lcd_Goto(0x40);
	Lcd_Puts(LINEA_2);
	return;
}

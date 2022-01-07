/*
 *	LCD interface header file
 *	See lcd.c for more info
 */

/* write a byte to the LCD in 4 bit mode */

extern void Lcd_Write(unsigned char);

/* Clear and home the LCD */

extern void Lcd_Clear(void);

/* write a string of characters to the LCD */

extern void Lcd_Puts(const char * s);

/* Go to the specified position */

extern void Lcd_Goto(unsigned char pos);

/* intialize the LCD - call before anything else */

extern void Lcd_Init(void);

extern void ScrollMessage(unsigned char row,const char Message[]);

extern void Lcd_Putch(char c);

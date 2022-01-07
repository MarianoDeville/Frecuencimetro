#if ((PIC_CLK==4000000)	&& (RS232==1200))
	#define	cte	207
#elif ((PIC_CLK==4000000) && (RS232==2400))
	#define	cte	103
#elif ((PIC_CLK==4000000) && (RS232==9600))
	#define	cte	25
#elif ((PIC_CLK==4000000) && (RS232==19200))
	#define	cte	12
#elif ((PIC_CLK==4000000) && (RS232==28800))
	#define	cte	8
#elif ((PIC_CLK==4000000) && (RS232==33600))
	#define	cte	6
#elif ((PIC_CLK==4000000) && (RS232==57600))
	#define	cte	3

#elif ((PIC_CLK==10000000) && (RS232==2400))
	#define	cte	255
#elif ((PIC_CLK==10000000) && (RS232==9600))
	#define	cte	64
#elif ((PIC_CLK==10000000) && (RS232==19200))
	#define	cte	31
#elif ((PIC_CLK==10000000) && (RS232==28800))
	#define	cte	21
#elif ((PIC_CLK==10000000) && (RS232==33600))
	#define	cte	18
#elif ((PIC_CLK==10000000) && (RS232==57600))
	#define	cte	10

#elif ((PIC_CLK==16000000) && (RS232==9600))
	#define	cte	103
#elif ((PIC_CLK==16000000) && (RS232==19200))
	#define	cte	51
#elif ((PIC_CLK==16000000) && (RS232==28800))
	#define	cte	33
#elif ((PIC_CLK==16000000) && (RS232==33600))
	#define	cte	29
#elif ((PIC_CLK==16000000) && (RS232==57600))
	#define	cte	16

#elif ((PIC_CLK==20000000) && (RS232==9600))
	#define	cte	129
#elif ((PIC_CLK==20000000) && (RS232==19200))
	#define	cte	64
#elif ((PIC_CLK==20000000) && (RS232==28800))
	#define	cte	42
#elif ((PIC_CLK==20000000) && (RS232==33600))
	#define	cte	36
#elif ((PIC_CLK==20000000) && (RS232==57600))
	#define	cte	20

#else
	#error rs232.h - por favor definir PIC_CLK y RS232 correctamente.
	#error Posibles PIC_CLK 4000000 10000000 16000000 20000000
	#error Posibles RS232 1200 2400 9600 19200 28800 33600 57600

#endif

/*12864 ���������*/
#include "lcd12864.h"
#include "msp430.h"

void write_byte(uchar byte)		//д�ֽں���
{ 
	uchar i; 
	for(i=0;i<8;i++) 
	{ 
		sclk_0; 
                if(byte & 0x80)
                  sid_1;
                else
                  sid_0;
		byte<<=1; 
		sclk_1; 
	} 
} 

uchar read_byte()		//���ֽں���
{ 
	uchar i,temp1,temp2; 
	temp1 = 0; 
	temp2 = 0; 
	for(i=0;i<8;i++)		//������λ
	{ 
		temp1=temp1<<1; 
		sclk_0;
		sclk_1; 
		sclk_0; 
                sid_in;
                nop;
                nop;
		if(lcdin & sid) 
			temp1++; 
                sid_out;
                nop;
                nop;
	} 
	for(i=0;i<8;i++)
	{ 
		temp2=temp2<<1; 
		sclk_0; 
		sclk_1; 
		sclk_0; 
                sid_in;
                nop;
                nop;
		if(lcdin & sid) 
			temp2++; 
                sid_out;
                nop;
                nop;
	}
	return ((0xf0&temp1)+(0x0f&temp2)); 
}

void lcd_busy()
{ 
	do write_byte(0xfc);	//11111,A(1),B(0),0 
	while(0x80&read_byte());	 
}

void write(uchar dat_com,uchar byt)
{ 
	uchar temp; 
	if(dat_com==0)
		temp=0xf8; //11111,A(0),B(0),0 
	else
		temp=0xfa;  //11111,A(0),B(1),0
	cs_1; 
	lcd_busy(); 
	write_byte(temp); 
	write_byte(byt&0xf0);
	write_byte(byt<<4);
	cs_0;
} 

void lcd_init()
{
        lcddir |= cs+sid+sclk;
        lcdport |= cs+sid+sclk;
	write(com,0x30);
        delay_ms(2);
	write(com,0x0c);
        delay_ms(2);
	write(com,0x01);
        delay_ms(2);
	write(com,0x06);
        delay_ms(2);
} 
 
void lcd_clear()
{ 
	write(com,0x30); 
	write(com,0x01); 
}

void write_zi(uchar x,uchar y,uchar *buff)
{ 
	uchar addr,i=0; 
        switch(x)
        {
        case 0: addr=0x80;break;
        case 1: addr=0x90;break;
	case 2:	addr=0x88;break;
	case 3: addr=0x98;break;
	default: break;
        }   
	addr=addr+y;
	write(com,0x30);
	write(com,addr);
	while(buff[i]!='\0') 
	{ 
		write(dat,buff[i]); 
		i++;
	}
}
void write_char(uchar x,uchar y,uchar data)
{
	uchar addr;
	switch(x)
	        {
	        case 0: addr=0x80;break;
	        case 1: addr=0x90;break;
		case 2:	addr=0x88;break;
		case 3: addr=0x98;break;
		default: break;
	        }
		addr=addr+y;
		write(com,0x30);
		write(com,addr);
		write(dat,data);
}

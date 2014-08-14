#ifndef LCD12864_H
#define LCD12864_H

#include "global_define.h"

#define com 0 
#define dat 1

#define cs   BIT2
#define sid  BIT3
#define sclk BIT4
#define lcdport P1OUT
#define lcddir P1DIR
#define lcdin P1IN
#define sid_out lcddir |= sid
#define sid_in lcddir &= ~sid
#define sid_1  lcdport |= sid
#define sid_0  lcdport &= ~sid
#define sclk_1 lcdport |= sclk
#define sclk_0 lcdport &= ~sclk
#define cs_1   lcdport |= cs
#define cs_0   lcdport &= ~cs

void write_byte(uchar byte);		//д�ֽں���
uchar read_byte();		//���ֽں���
void lcd_busy();	//���æ����
void write(uchar dat_com,uchar byt); //д�뺯��(��дcom��dat����д����ָ������)

/*���¸�����Ϊ��������������ú���*/
void lcd_init();		//��ʼ������
void lcd_clear();	//��������
void write_zi(uchar x,uchar y,uchar *buff);		//���������������������ַ��������
void write_char(uchar x,uchar y,uchar data);

#endif

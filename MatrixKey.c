/*----------------------------------------------------------------------------------------------------------
  文件名  :  MatrixKey.c   ( msp430版 )
  描述    :  4*4矩阵键盘驱动包	V 0.1
  日期	  :  2014-8-8
  制作人  :  Beny
----------------------------------------------------------------------------------------------------------*/
#include <msp430.h>
//#include "delay.h"
#include "MatrixKey.h"



#define MODE_1_MASK		0xf0
#define MODE_2_MASK		0x0f
//#define MODE_3_MASK		0xff

//  矩阵键盘的端口映射-----------------------------------------------------------------

#define KEY_PORT_OUT				P3OUT				//
#define KEY_PORT_IN					P3IN
#define KEY_PORT_REN				P3REN

#define	KEY_PORT_SCAN_MODE_1	P3DIR = (MODE_1_MASK)	// 低位（行）设为输出（扫描），高位（列）设为输入（检测）
#define	KEY_PORT_SCAN_MODE_2	P3DIR = (MODE_2_MASK)	// 低位（行）设为输入（检测），高位（列）设为输出（扫描）
//#define	KEY_PORT_SCAN_MODE_3	TRISB = MODE_3_MASK	// 低位（行）设为输入（检测），高位（列）设为输出（扫描）
//-------------------------------------------------------------------------------------


//  函数声明---------------------------------------------------------------------------
unsigned char KeyScan (void);
unsigned char GetKeyValue (void);
//-------------------------------------------------------------------------------------




/*-------------------------------------------------------------------------------------
     矩阵键盘的 按键检测 
			基于状态机思想 编写的矩阵键盘 扫描函数 支持 长按 连发功能		  
  			无按键按下  则返回   数值 NO_KEY_PRESS(20)
--------------------------------------------------------------------------------------*/
void key_Init()
{
	TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA1CCR0 = 4000-1; 						//1ms
	TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, contmode, clear TAR
	_EINT();
}

unsigned char KeyScan ( void )
{
	unsigned char curKeyValue = NO_KEY_PRESS;           	// 当前读到的  键值
	static unsigned char keyValueBackup = NO_KEY_PRESS;		// 键值的 备份
    static unsigned int keyPressTimer = 0;              	// 按键按下的  时间值
    static unsigned int keyQuickTimer = 0;					// 按键连发的时间间隔
    static unsigned char curStatus = KEY_STATUS_noKeyPress; // 当前的状态
	unsigned char returnKeyValue; // 返回 的键值

	/* -----获取键值----- */
	curKeyValue = GetKeyValue();		

	switch(curStatus) {
		/* -----无按键按下时的状态----- */
		case KEY_STATUS_noKeyPress: 
			if( curKeyValue != NO_KEY_PRESS ) {// 判断是否有按键按下 有则进入 按键消抖状态 无则 保持当前状态
				keyValueBackup = curKeyValue;
				curStatus = KEY_STATUS_filter; 					
			}
			returnKeyValue = NO_KEY_PRESS;
			break;
		/* -----消抖状态----- */	
		case KEY_STATUS_filter:
			if (curKeyValue == NO_KEY_PRESS) {
				curStatus = KEY_STATUS_Release;
			}else {
				if( ++keyPressTimer > KEY_FILTER_TIME ) {	// 判断消抖是否完成 完成则进入 按键单击状态 否则保持当前状态
					returnKeyValue = curKeyValue;
					curStatus = KEY_STATUS_clicking;
				}
				else
					returnKeyValue = NO_KEY_PRESS; //返回 键值
			}
				break;
		/* -----单击状态----- */	
		case KEY_STATUS_clicking: 
			if( curKeyValue == NO_KEY_PRESS )	// 判断按键是否 抬起 是则 进入 按键释放状态，否则 判断是否 长按 是则进入长按状态
				curStatus = KEY_STATUS_Release; 
			else if( ++keyPressTimer > LONG_PRESS_TIME )			
				curStatus = KEY_STATUS_longKeyPress; 
			returnKeyValue = NO_KEY_PRESS; 
			break;
		/* -----按键连发状态----- */	
		case KEY_STATUS_longKeyPress: 		
			if( curKeyValue == NO_KEY_PRESS ) {	// 判断按键是否 抬起 是则 进入 按键释放状态，否则 按键连发 
				curStatus = KEY_STATUS_Release;	
				returnKeyValue = NO_KEY_PRESS; 
			}
			else if( ++keyQuickTimer > QUICK_ACT_SPACE ) {
				keyQuickTimer = 0;
				returnKeyValue = curKeyValue;
			}
			break;
		/* -----按键释放状态----- */	
		case KEY_STATUS_Release:
			keyValueBackup = NO_KEY_PRESS;
			keyPressTimer = 0;
			keyQuickTimer =0;
			curStatus = KEY_STATUS_noKeyPress;			
			break;	
		/* ----- ----- */
		default:
			break;
	}
	/* -----返回键值----- */
	return returnKeyValue;	
}

/*-------------------------------------------------------------------------------------
    矩阵键盘扫描并返回相应的键值		hang(0~3)-> 0  1  2  3
										lie(0~3)	4  5  6  7
    按键接口 (KEY_PORT)  					^		8  9  10 11
											|		12 13 14 15
											|		16 17 18 19
--------------------------------------------------------------------------------------*/
unsigned char GetKeyValue ( void )
{
	volatile unsigned char hangTemp,lieTemp,key_value;
	
	/* ----- 确定 第几列----- */	
	KEY_PORT_REN = 0xff;
	P3DS = 0xff;
	P3DIR = 0xF0;
	P3OUT = 0xf0;
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	lieTemp = (~P3IN) & 0x0F;

	//KEY_PORT_SCAN_MODE_1;	//TRISB = 0xf0;
	//KEY_PORT_OUT = ~MODE_1_MASK;
	//_NOP();
	//_NOP();
	//lieTemp = (KEY_PORT_IN & (~MODE_1_MASK));
		
	/* -----确定 第几行----- */

	P3REN = 0xff;
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	P3DIR = 0x0f;
	P3OUT = 0xf0;
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	_NOP();
	hangTemp = P3IN & 0xf0;

	//KEY_PORT_REN = 0xff;
	//KEY_PORT_SCAN_MODE_2;	//TRISB = 0x0f;
	//KEY_PORT_OUT = ~MODE_2_MASK;
	//_NOP();
	//_NOP();
	//hangTemp = KEY_PORT_IN & (~MODE_2_MASK);
	/* -----把行列组合起来 确定键值----- */
	key_value = (lieTemp | hangTemp);
	switch( key_value ) {
		case 0xee : key_value = 0; break;
    		case 0xde : key_value = 1; break;
		case 0xbe : key_value = 2; break;
	    	case 0x7e : key_value = 3; break;
    		case 0xed : key_value = 4; break;
	    	case 0xdd : key_value = 5; break;
    		case 0xbd : key_value = 6; break;
	    	case 0x7d : key_value = 7; break;
	    	case 0xeb : key_value = 8; break;
	    	case 0xdb : key_value = 9; break;
    		case 0xbb : key_value = 10;break;
    		case 0x7b : key_value = 11;break;
    		case 0xe7 : key_value = 12;break;
    		case 0xd7 : key_value = 13;break;
    		case 0xb7 : key_value = 14;break;
    		case 0x77 : key_value = 15;break;
    		case 0xff : 
    		default :   key_value = NO_KEY_PRESS;
	}
	/* -----返回 键值----- */
	return key_value;
}




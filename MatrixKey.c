/*----------------------------------------------------------------------------------------------------------
  �ļ���  :  MatrixKey.c   ( msp430�� )
  ����    :  4*4����������	V 0.1
  ����	  :  2014-8-8
  ������  :  Beny
----------------------------------------------------------------------------------------------------------*/
#include <msp430.h>
//#include "delay.h"
#include "MatrixKey.h"



#define MODE_1_MASK		0xf0
#define MODE_2_MASK		0x0f
//#define MODE_3_MASK		0xff

//  ������̵Ķ˿�ӳ��-----------------------------------------------------------------

#define KEY_PORT_OUT				P3OUT				//
#define KEY_PORT_IN					P3IN
#define KEY_PORT_REN				P3REN

#define	KEY_PORT_SCAN_MODE_1	P3DIR = (MODE_1_MASK)	// ��λ���У���Ϊ�����ɨ�裩����λ���У���Ϊ���루��⣩
#define	KEY_PORT_SCAN_MODE_2	P3DIR = (MODE_2_MASK)	// ��λ���У���Ϊ���루��⣩����λ���У���Ϊ�����ɨ�裩
//#define	KEY_PORT_SCAN_MODE_3	TRISB = MODE_3_MASK	// ��λ���У���Ϊ���루��⣩����λ���У���Ϊ�����ɨ�裩
//-------------------------------------------------------------------------------------


//  ��������---------------------------------------------------------------------------
unsigned char KeyScan (void);
unsigned char GetKeyValue (void);
//-------------------------------------------------------------------------------------




/*-------------------------------------------------------------------------------------
     ������̵� ������ 
			����״̬��˼�� ��д�ľ������ ɨ�躯�� ֧�� ���� ��������		  
  			�ް�����  �򷵻�   ��ֵ NO_KEY_PRESS(20)
--------------------------------------------------------------------------------------*/
void key_Init()
{
	TA1CCTL0 = CCIE;                          // CCR0 interrupt enabled
	TA1CCR0 = 40000-1; 						//10ms
	TA1CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, contmode, clear TAR
	_EINT();
}

unsigned char KeyScan ( void )
{
	unsigned char curKeyValue = NO_KEY_PRESS;           	// ��ǰ������  ��ֵ
	static unsigned char keyValueBackup = NO_KEY_PRESS;		// ��ֵ�� ����
    static unsigned int keyPressTimer = 0;              	// �����µ�  ʱ��ֵ
    static unsigned int keyQuickTimer = 0;					// ����������ʱ����
    static unsigned char curStatus = KEY_STATUS_noKeyPress; // ��ǰ��״̬
	unsigned char returnKeyValue; // ���� �ļ�ֵ

	/* -----��ȡ��ֵ----- */
	curKeyValue = GetKeyValue();		

	switch(curStatus) {
		/* -----�ް�����ʱ��״̬----- */
		case KEY_STATUS_noKeyPress: 
			if( curKeyValue != NO_KEY_PRESS ) {// �ж��Ƿ��а����� ������� ������״̬ ���� ���ֵ�ǰ״̬
				keyValueBackup = curKeyValue;
				curStatus = KEY_STATUS_filter; 					
			}
			returnKeyValue = NO_KEY_PRESS;
			break;
		/* -----��״̬----- */	
		case KEY_STATUS_filter:
			if (curKeyValue == NO_KEY_PRESS) {
				curStatus = KEY_STATUS_Release;
			}else {
				if( ++keyPressTimer > KEY_FILTER_TIME ) {	// �ж����Ƿ���� �������� �����״̬ ���򱣳ֵ�ǰ״̬
					returnKeyValue = curKeyValue;
					curStatus = KEY_STATUS_clicking;
				}
				else
					returnKeyValue = NO_KEY_PRESS; //���� ��ֵ
			}
				break;
		/* -----����״̬----- */	
		case KEY_STATUS_clicking: 
			if( curKeyValue == NO_KEY_PRESS )	// �жϰ����Ƿ� ̧�� ���� ���� �����ͷ�״̬������ �ж��Ƿ� ���� ������볤��״̬
				curStatus = KEY_STATUS_Release; 
			else if( ++keyPressTimer > LONG_PRESS_TIME )			
				curStatus = KEY_STATUS_longKeyPress; 
			returnKeyValue = NO_KEY_PRESS; 
			break;
		/* -----��������״̬----- */	
		case KEY_STATUS_longKeyPress: 		
			if( curKeyValue == NO_KEY_PRESS ) {	// �жϰ����Ƿ� ̧�� ���� ���� �����ͷ�״̬������ �������� 
				curStatus = KEY_STATUS_Release;	
				returnKeyValue = NO_KEY_PRESS; 
			}
			else if( ++keyQuickTimer > QUICK_ACT_SPACE ) {
				keyQuickTimer = 0;
				returnKeyValue = curKeyValue;
			}
			break;
		/* -----�����ͷ�״̬----- */	
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
	/* -----���ؼ�ֵ----- */
	return returnKeyValue;	
}

/*-------------------------------------------------------------------------------------
    �������ɨ�貢������Ӧ�ļ�ֵ		hang(0~3)-> 0  1  2  3
										lie(0~3)	4  5  6  7
    ����ӿ� (KEY_PORT)  					^		8  9  10 11
											|		12 13 14 15
											|		16 17 18 19
--------------------------------------------------------------------------------------*/
unsigned char GetKeyValue ( void )
{
	volatile unsigned char hangTemp,lieTemp,key_value;
	
	/* ----- ȷ�� �ڼ���----- */	
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
		
	/* -----ȷ�� �ڼ���----- */

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
	/* -----������������� ȷ����ֵ----- */
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
	/* -----���� ��ֵ----- */
	return key_value;
}




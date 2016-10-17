#include "stm32f1xx_hal.h"
#include "stdlib.h"
#include "lcd.h"
#include "key.h"
#include "ff.h"
#include "file_ex.h"
#include "nRF24L01.h"
/* Usr Define */
#define LCD_TEXT(x) (const unsigned char*)(x)

#if (DEBUG_WITHOUT_SLAVE)
  #define __SEND HAL_Delay(1);
#else
  #define __SEND while (nRF24L01_TxPack(&tpt) != _SET) { nRF24L01_Channel_Init(40); }//HAL_Delay(4);}
#endif
	
/* Wirless private var */
static const uint8_t addr[5] = {0x34,0x43,0x10,0x10,0x01};
const uint8_t        start_flag_type1[] = "A";
const uint8_t        start_flag_type2[] = "B";
const uint8_t        eof_flag[]   = "EOF";
nRF24L01_TxStructure tpt;

/* LCD private var */
extern void          LCD_ShowUsrFont24(unsigned short x, unsigned short y, const char *src);
extern void          LCD_Draw_ATD_Black(unsigned char x, unsigned short y, unsigned char *src);
extern void          LCD_Draw_ATD_Red(unsigned char x, unsigned short y, unsigned char *src);
extern const char    font_ch[][72];
static uint8_t       File_Buffer[120000 / 5];

/* Timer private var*/
__IO uint8_t         Usr_Cancel_Flag = 0;
__IO uint32_t        TIMEOUT_cnt = 0;//uesed in Systick irq

/**
  * @Brief: check and init file system and wirless moudel
  */
uint32_t Unity_check(void) {
	uint8_t status = 0x00;
	
	LCD_Init();
	LCD_Clear(GREEN);
	POINT_COLOR = BLACK;
	LCD_ShowString(0, 0, LCD_TEXT("Version V3.0"));
	
	/* check infomation show on LCD */
	if (file_init()) {
		POINT_COLOR = RED;
		LCD_ShowString(0, 20, LCD_TEXT("File System Error!"));
		status = 1;
	} else {
		POINT_COLOR = BLACK;
		LCD_ShowString(0, 20, LCD_TEXT("File System OK!"));
	}
	
	if (nRF24L01_Check() != 1) {
		POINT_COLOR = RED;
		LCD_ShowString(0, 40, LCD_TEXT("Wirless module Error!"));
		status = 1;
	} else {
		POINT_COLOR = BLACK;
		LCD_ShowString(0, 40, LCD_TEXT("Wirless module OK!"));
	}
	
	/* if some thing wrong, stop at there */
	while (status);
	
	/* nRF24L01 check ok, then init moudel */
	nRF24L01_Init();
	nRF24L01_Channel_Init(40);
	nRF24L01_TxInit((uint8_t*)addr);
	nRF24L01_RxInit(P0, (uint8_t*)addr);
	
	/* file system has already init in file_init() */
	return 0x00;
}

/**
  * @Week handle, called by nRF24L01_Tx()
  */
unsigned char Wirless_WaitingTx_CancelHook(void) {
  /* Press Cancel Key, then exit */
  if( HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port, Usr_Key_Right_Pin) == GPIO_PIN_RESET) {
    Usr_Cancel_Flag = 1;
    return 1;
  }
  /* Time out */
  if (TIMEOUT_cnt > 1000) {
    Usr_Cancel_Flag = 2;
    return 1;
  }
  return 0;
}
static void File_Trasmit(uint32_t file_loc) {
	FIL file;
	uint32_t file_id = 0;
	/* Power Up the Slave */
	HAL_GPIO_WritePin(SLAVE_POWER_SWITCH_GPIO_Port,
	                  SLAVE_POWER_SWITCH_Pin,
	                  GPIO_PIN_RESET);
	/* Led up */
	HAL_GPIO_WritePin(Usr_Led_0_GPIO_Port,
	                  Usr_Led_0_Pin,
	                  GPIO_PIN_SET);
	/* Waiting for slave ack */
	HAL_Delay(100);
	
	/* sending... GUI*/
	POINT_COLOR = RED;
	LCD_DrawLine(75, 260, 75, 320);
	POINT_COLOR = BLACK;
	
	LCD_ShowUsrFont24(120, 270, font_ch[4]);
	LCD_ShowUsrFont24(144, 270, font_ch[5]);
	LCD_ShowUsrFont24(168, 270, font_ch[6]);
	
	/* open file */
	file_open(file_loc, &file);
	
	/* get file type */
	file_id = file_type(file_loc);
	/* id = 0x01 - DTA type, Byte:120000
	   id = 0x02 - ATD type, Byte:30000
	*/
	
	/*sending...*/
	{
		uint32_t i,j;
		UINT     cnt;
		uint32_t Byte = 0;
		uint32_t loop = 0;
		
		/* send start flag */
		TIMEOUT_cnt = 0;
		switch (file_id) {
			case 0x01:
				tpt.pSrc = (uint8_t*)start_flag_type1;
				break;
			case 0x02:
				tpt.pSrc = (uint8_t*)start_flag_type2;
				break;
		}
		tpt.Txnum = 1;
		__SEND
		
		switch (file_id) {
				case 0x01:
				loop = 5;
				Byte = 120000/5;
				break;
				case 0x02:
				loop = 2;
				Byte = 15000;
				break;
				default:
				break;
		}
		/* read file and send */
		for (i = 0; i < loop; i++) {
			f_read(&file, File_Buffer, Byte, &cnt);
			tpt.pSrc = File_Buffer;
			tpt.Txnum = 32;
			for (j = 0; j < Byte/32; j++) {
				
				/* send data */
				TIMEOUT_cnt = 0;
				tpt.pSrc += 32;
				__SEND;
				
				/* 进度条 */
				/*
				i_per = 75/loop
				j_per = j_per * (32 / Byte) */
				LCD_DrawLine(i*75/loop + j*75/loop*32/Byte, 280, i*75/loop + j*75/loop*32/Byte, 300);
				
				/* Chancel and error */
				if (Usr_Cancel_Flag == 2) {
					LCD_Fill(80, 270, 240, 304, WHITE);
					/* 发送失败 */
					LCD_ShowUsrFont24(20 + 80, 270, font_ch[4]);
					LCD_ShowUsrFont24(20 + 104, 270, font_ch[5]);
					LCD_ShowUsrFont24(20 + 128, 270, font_ch[7]);
					LCD_ShowUsrFont24(20 + 152, 270, font_ch[8]);
					Usr_Cancel_Flag = 1;
					while (HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port,
						                      Usr_Key_Right_Pin) == GPIO_PIN_SET);
				}
				if (Usr_Cancel_Flag == 1) {
					Usr_Cancel_Flag = 0;
					goto ENDOFSENDING;
				}

			}
		}
		TIMEOUT_cnt = 0;
		tpt.pSrc = (uint8_t*)eof_flag;
		tpt.Txnum = 3;
		__SEND;/*sending....end*/
		
		switch (file_id) {
			case 0x01:
				/* Waiting (4.5s) Slave complete*/
				HAL_Delay(4500);
				break;
			
			case 0x02:
				/* Waiting (8.0s) Slave complete*/
				HAL_Delay(8000);
				break;
		}
	} /* send handle */

	ENDOFSENDING:
	
	/* close file*/
	f_close(&file);
	
	/* Led down */
	HAL_GPIO_WritePin(Usr_Led_0_GPIO_Port,
	                  Usr_Led_0_Pin,
	                  GPIO_PIN_RESET);
	/* Power Down the Slave*/
	HAL_GPIO_WritePin(SLAVE_POWER_SWITCH_GPIO_Port,
	                  SLAVE_POWER_SWITCH_Pin,
	                  GPIO_PIN_SET);
}
void Unit_Handle(void *arg) {
	static uint32_t mode = 1;
	static uint32_t file_loc = 1;
	static uint32_t res  = 0;
	static uint32_t i    = 0;
	static FRESULT  efs;
	static UINT     fnt = 0;
	static FIL      file;
	static KeyValueStructure key_val;
	
	switch (mode) {
		
		/* mode = 1
		   图形绘制相关
		   完成绘制后转到按键检测
		*/
		case 1:
			LCD_Clear(WHITE);
			POINT_COLOR = BLACK;
			
			/**
		    * 绘制函数
		   */
			/* 绘制确定 */
			LCD_ShowUsrFont24(10, 190, font_ch[0]);
			LCD_ShowUsrFont24(34, 190, font_ch[1]);
			/* 绘制取消 */
			LCD_ShowUsrFont24(180, 190, font_ch[2]);
			LCD_ShowUsrFont24(204, 190, font_ch[3]);
			/* Draw a line */
			LCD_DrawLine(0, 220, 240, 220);
		
			/* Open file */
			res = file_open(file_loc, &file);
			/* identy id */
			res = file_type(file_loc);
		
			if (efs != 0x00)
				LCD_ShowString(0, 0, LCD_TEXT("Open file Error!"));

			/* file type opra */
			switch (res) {
				/* .DTA black-white four Layer */
				case 0x01:
					efs = f_lseek(&file, 32);
					for (i = 0; i < 5; i++) {
						efs = f_read(&file, File_Buffer, 120000 / 5, &fnt);
						LCD_ShowPicture(20, 20 + 30*i, File_Buffer);
					}
					
					break;
				/* .ATD red-black-white one Layer */
				case 0x02:
					/* blace pic first */
					efs = f_read(&file, File_Buffer, 15000, &fnt);
					LCD_Draw_ATD_Black(20, 15, File_Buffer);
					efs = f_read(&file, File_Buffer, 15000, &fnt);
					LCD_Draw_ATD_Red(20, 15, File_Buffer);
					break;
				default:
					break;
			} /* end of switch(res) */
			
			/* close file */
			efs = f_close(&file);
			
			/* goto mode 2*/
			mode = 2;
			break;
			
        /* mode = 2
           按键检测部分
		*/
		case 2:
			key_val = Usr_Key_Scan();
			
			if (key_val == KEY_DOWN) {
				if (++file_loc > number_of_file())
					file_loc = 1;
				mode = 1;
			}
			else if (key_val == KEY_UP) {
				if (--file_loc <= 0) {
					file_loc = number_of_file();
				}
				mode = 1;
			}
			else if (key_val == KEY_LEFT) {
				mode = 3;
			}
			break;
		
		/* mode = 3
			 文件发送模式
		*/
		case 3:
			File_Trasmit(file_loc);
			mode = 1;
			break;
		
		default:
			mode = 1;
			break;
			
	}
}

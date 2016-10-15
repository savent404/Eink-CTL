#include "key.h"

//void Usr_Key_Init(void){
//    GPIO_InitTypeDef GPIO_InitStruct = {
//        .Pin = GPIO_PIN_0 | GPIO_PIN_1 |\
//               GPIO_PIN_2 | GPIO_PIN_3,
//        .Mode = GPIO_MODE_INPUT,
//        .Pull = GPIO_PULLUP,
//    };
//    
//    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//}

KeyValueStructure Usr_Key_Scan(void){
    static unsigned char status = 0x00;
    unsigned char buff = 0, ans;
    
    buff |= HAL_GPIO_ReadPin(Usr_Key_Down_GPIO_Port,
						Usr_Key_Down_Pin);
    buff <<= 1;
    buff |= HAL_GPIO_ReadPin(Usr_Key_Up_GPIO_Port,
						Usr_Key_Up_Pin);
    buff <<= 1;
    buff |= HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port,
						Usr_Key_Right_Pin);
    buff <<= 1;
    buff |= HAL_GPIO_ReadPin(Usr_Key_Left_GPIO_Port,
						Usr_Key_Left_Pin);
  
    HAL_Delay(5);
  
    buff &= HAL_GPIO_ReadPin(Usr_Key_Down_GPIO_Port,
						Usr_Key_Down_Pin) << 3 | 0xf7;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Up_GPIO_Port,
						Usr_Key_Up_Pin) << 2 | 0xfb;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port,
						Usr_Key_Right_Pin) << 1 | 0xfD;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Left_GPIO_Port,
						Usr_Key_Left_Pin) << 0 | 0xfE;
            
    HAL_Delay(5);
  
    buff &= HAL_GPIO_ReadPin(Usr_Key_Down_GPIO_Port,
						Usr_Key_Down_Pin) << 3 | 0xf7;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Up_GPIO_Port,
						Usr_Key_Up_Pin) << 2 | 0xfb;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port,
						Usr_Key_Right_Pin) << 1 | 0xfD;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Left_GPIO_Port,
						Usr_Key_Left_Pin) << 0 | 0xfE;
    HAL_Delay(5);
  
    buff &= HAL_GPIO_ReadPin(Usr_Key_Down_GPIO_Port,
						Usr_Key_Down_Pin) << 3 | 0xf7;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Up_GPIO_Port,
						Usr_Key_Up_Pin) << 2 | 0xfb;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Right_GPIO_Port,
						Usr_Key_Right_Pin) << 1 | 0xfD;
    buff &= HAL_GPIO_ReadPin(Usr_Key_Left_GPIO_Port,
						Usr_Key_Left_Pin) << 0 | 0xfE;
    
    ans = buff ^ status;
    ans = ans & status;
    status = buff;
    
    if (ans & 0x01){
        return KEY_LEFT;
    }
    else if (ans & 0x02){
        return KEY_RIGHT;
    }
    else if (ans & 0x04){
        return KEY_UP;
    }
    else if (ans & 0x08){
        return KEY_DOWN;
    }
		else {
        return KEY_NONE;
		}
			
}

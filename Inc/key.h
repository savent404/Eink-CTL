#ifndef _USR_KEY_H_
#define _USR_KEY_H_

#include "stm32f1xx_hal.h"

#define USR_KEY_UP
#define USR_KEY_DOWN
#define USR_KEY_RIGH
#define USR_KEY_LEFT
typedef enum {KEY_NONE, KEY_UP, KEY_DOWN, KEY_RIGHT,\
KEY_LEFT} KeyValueStructure;

void Usr_Key_Init(void);
KeyValueStructure Usr_Key_Scan(void);

#endif

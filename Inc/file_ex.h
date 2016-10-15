#ifndef _file_ex_H_
#define _file_ex_H_
#include "ff.h"
#include "string.h"
#include "stm32f1xx_hal.h"

/**
  * @Retvl: @0x01 SD card open error
	          @0x02 FATFS dir open error
	*/
uint8_t file_init(void);

/**
  * @Retvl: \file_pt pointer of FIL structure
    @Retvl: @0x01 open dir error
            @0x02 readdir func error
            @0x03 open file error
  */
uint8_t file_open(uint32_t loc,FIL* file_pt);

/**
  * @Retvl: number of file in "0:PICDTA/"
  */
uint32_t number_of_file(void);

/**
  * @Retvl: \type of file
            @0x00 Unknow type
            @0x01 BLACK-WHITE(Four Layer) file
            @0x02 RED-BLACK-WHITE(One Layer) file
  */
uint32_t file_type(uint32_t loc);

#endif

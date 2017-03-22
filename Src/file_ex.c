#include "file_ex.h"

static FRESULT          fs_err;
static FATFS            fs;
static DIR              fs_dir;
static FILINFO          fs_fileinfo;
static uint32_t         file_max_num;
const  TCHAR dir[] = "0:/PICDTA";

const  TCHAR Type_str[][4] = {"DTA", "ATD"};
/**
  * @Retvl: @0x01 SD card open error
	        @0x02 FATFS dir open error
            @0x00 SD card Init OK
  */
uint8_t file_init(void) {
	fs_err = f_mount (&fs, "0:/", 1);
	
	/* sd card init error */
	if (fs_err != FR_OK) {
		return 0x01;
	}
	
	/* dir open error */
	fs_err = f_opendir (&fs_dir, "0:/PICDTA");
	if (fs_err != FR_OK) {
		return 0x02;
	}
	
	/* find file num */
	while (1) {
		fs_err = f_readdir(&fs_dir, &fs_fileinfo);
		if (fs_err != FR_OK || fs_fileinfo.fname[0] == 0)
			break;
		file_max_num++;
	}
	return 0x00;
}

/**
  * @Brief: Open Locked Dir, find file with item's location
  * @Para loc: the file's location in This Dir(0:/PICDTA)
  * @Retvl: \file_pt pointer of FIL structure
    @Retvl: @0x01 open dir error
            @0x02 readdir func error
            @0x03 open file error
  */
uint8_t file_open(uint32_t loc,FIL* file_pt) {
	TCHAR buf[30];

	fs_err = f_opendir(&fs_dir, dir);
	if (fs_err != FR_OK)
		return 0x01;
	
	while (loc--) {
		fs_err = f_readdir(&fs_dir, &fs_fileinfo);
		if (fs_err != FR_OK || fs_fileinfo.fname[0] == 0)
			return 0x02;
	}
	
	/* dir set */
	/* etc: "0:/PICDTA/xxx.dta */
	memset(buf, 0, 30);
	strcat(buf, dir);
	strcat(buf, "/");
	strcat(buf, fs_fileinfo.fname);
	
	fs_err = f_open(file_pt, buf, FA_READ);
	if (fs_err != FR_OK)
		return 0x03;
	
	return 0x00;
}

/**
  * @Brief: Get avaliable files number in The dir
  * @Retvl: number of file in "0:PICDTA/"
  */
uint32_t number_of_file(void) {
	return file_max_num;
}

/**
  * @Brief: get the type of surpported file type
  * @Retvl: \type of file
            @0x00 Unknow type
            @0x01 BLACK-WHITE(Four Layer) file
            @0x02 RED-BLACK-WHITE(One Layer) file
  */
uint32_t file_type(uint32_t loc) {
	char *pt;
	fs_err = f_opendir(&fs_dir, dir);
	while (loc--) {
		fs_err = f_readdir(&fs_dir, &fs_fileinfo);
	}
	pt = fs_fileinfo.fname;
	while (*pt != '.' && *pt != 0x00) {
		pt++;
	}
	
	
	if (!strcmp(Type_str[0], pt + 1)) {
		return 0x01;
	}
	else if (!strcmp(Type_str[1], pt + 1)) {
		return 0x02;
	}
	else {
		return 0x00;
	}
}

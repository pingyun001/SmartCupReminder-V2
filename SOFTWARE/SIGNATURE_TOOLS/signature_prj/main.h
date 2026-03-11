#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define SIM_BUFFER_SIZE 1024*1024*4
#define GLOBAL_FILE_NUM 100

extern uint8_t simBuffer[SIM_BUFFER_SIZE];

extern char *workSpaceOfPackFilePath;

#endif /* _MAIN_H_ */

#ifndef __WIN_HAL_H__
#define __WIN_HAL_H__

#include <stdint.h>
#include <stdio.h>
#include "main.h"

int32_t delFile(char *filename);

int32_t checkReadAndCreatFile(char *filename, uint8_t *outBuf, uint32_t size);

int32_t checkWriteAndCreateFile(char *filename, uint8_t *inBuf, uint32_t size);

void listFiles(char *path);

void listFilesAndSaveName(char *path, char allFilesName[GLOBAL_FILE_NUM][100]);

void printNameList(char allFilesName[GLOBAL_FILE_NUM][100]);

#endif //__WIN_HAL_H__

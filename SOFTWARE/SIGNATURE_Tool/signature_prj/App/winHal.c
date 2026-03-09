#include "winHal.h"
#include <stdio.h>
#include <windows.h>
#include <tchar.h>

int32_t checkReadAndCreatFile(char *filename, uint8_t *outBuf, uint32_t size)
{
    FILE* pf = fopen(filename,"rb");
	//判断是否打开成功
	if (pf == NULL)
	{
		printf("winHal: open file %s failed\n", filename);
        pf = fopen(filename,"wb+");
		if (pf == NULL)
		{
			printf("winHal: create file %s failed\n", filename);
			return 0;
		}
        printf("winHal: create file %s success\n", filename);
	}
    fseek(pf, 0, SEEK_SET);
    size_t res = fread(outBuf, 1, size, pf);
    printf("winHal: read file %s , length %d, success\n", filename, res);
    fclose(pf);
    return res;
}

int32_t checkWriteAndCreateFile(char *filename, uint8_t *inBuf, uint32_t size)
{
    FILE* pf = fopen(filename,"wb");
    if (pf == NULL)
	{
		perror("fopen");
		return 0;
	}
    size_t res = fwrite(inBuf, 1, size, pf);
    if (res != size)
    {
        perror("fwrite");
        return 0;
    }
    fclose(pf);
    printf("winHal: write file %s , length : %d, success\n", filename, res);
    return 1;
}

int32_t delFile(char *filename)
{
    return remove(filename);
}

void listFiles(char *path)
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char searchPath[1024];
    
    // 构建搜索路径
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
    
    // 查找第一个文件
    hFind = FindFirstFile(searchPath, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("can not open dir %s\n", path);
        return;
    }
    
    // 循环读取目录
    do
    {
        // 忽略当前目录和上级目录
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }
        
        // 构建完整路径
        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s\\%s", path, findFileData.cFileName);
        
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            // 是文件夹
            printf("folder: %s\n", findFileData.cFileName);
        }
        else
        {
            // 是文件
            printf("file: %s\n", findFileData.cFileName);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    
    // 关闭查找句柄
    FindClose(hFind);
}

void listFilesAndSaveName(char *path, char allFilesName[GLOBAL_FILE_NUM][100])
{
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    char searchPath[1024];
    int i = 0;
    
    // 构建搜索路径
    snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
    
    // 查找第一个文件
    hFind = FindFirstFile(searchPath, &findFileData);
    
    if (hFind == INVALID_HANDLE_VALUE)
    {
        printf("can not open dir %s\n", path);
        return;
    }
    
    // 循环读取目录
    do
    {
        // 忽略当前目录和上级目录
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0)
        {
            continue;
        }
        
        // 构建完整路径
        char file_path[1024];
        snprintf(file_path, sizeof(file_path), "%s\\%s", path, findFileData.cFileName);
        
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            // 是文件
            if (i < GLOBAL_FILE_NUM)
            {
                strcpy(allFilesName[i], findFileData.cFileName);
                i++;
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0 && i < GLOBAL_FILE_NUM);
    
    // 关闭查找句柄
    FindClose(hFind);
}

void printNameList(char allFilesName[GLOBAL_FILE_NUM][100])
{
    int i = 0;
    while (i < GLOBAL_FILE_NUM)
    {
        if(allFilesName[i][0] == '\0')
            break;

        printf("nameList[ %d ] = %s\n", i, allFilesName[i]);
        i++;
    }
}


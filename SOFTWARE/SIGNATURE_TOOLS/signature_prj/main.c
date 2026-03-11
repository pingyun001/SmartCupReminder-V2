#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "winHal.h"
#include "binSignature.h"

uint8_t testbuf[10 * 1024 * 1024] = {0};

int main(int argc, char *argv[])
{
    printf("\n>>>Lime Signature Tool V1.0\n");
    printf(">>>CompileTime:%s %s\n", __DATE__, __TIME__);

    //判断参数个数
    if (argc != 3) 
    {
        printf("how to use: %s <file path> <version(uint32_t hex)>\n", argv[0]);
        printf("example:  %s data.bin 0x12345678\n", argv[0]);
        return 1;
    }

    //获取输入bin路径
    char filePath[1024] = {0};
    snprintf(filePath, sizeof(filePath), "%s", argv[1]);

    //获取版本号
    uint32_t version = strtoul(argv[2], NULL, 0);
    printf(">>>cmd version:0x%08x\n", version);

    //计算输出bin路径
    char fileOutPath[1024 + 100] = {0};
    memcpy(fileOutPath, filePath, strlen(filePath) - 4);
    strcat(fileOutPath, "_signed.bin");
    printf(">>>cmd out filePath:%s\n", fileOutPath);

    //读取固件
    uint32_t readded = checkReadAndCreatFile(filePath, testbuf, sizeof(testbuf));
    printf(">>>readded(raw bin len): %d bytes\n", readded);

    //判断固件是否已经被签名
    bool res1 = bin_check_signature(testbuf, NULL);
    if (res1)
    {
        printf(">>>bin already signed\n");
        return 0;
    }
    

    //签名固件
    uint32_t res_bin_len = bin_signature(testbuf, readded, version);
    printf(">>>res_bin_len: %d bytes\n", res_bin_len);
    if(res_bin_len == 0)
    {
        printf(">>>signature fail\n");
        return 0;
    }

    //校验签名结果
    bool res2 = bin_check_signature(testbuf, NULL);
    if(res2 == 0)
    {
        printf(">>>check signature fail\n");
        return 0;
    }
    
    //导出签名后的固件
    checkWriteAndCreateFile(fileOutPath, testbuf, res_bin_len);

    printf(">>>finish\n\n");

    return 0;
}


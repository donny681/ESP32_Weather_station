/*
 * my_url_parser.c
 *
 *  Created on: Dec 18, 2019
 *      Author: fyy
 */

/* 这是一份描述从Http请求信息中提取有效键值的接口定义文件 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "my_url_parser.h"

/*
 * 内部接口
 *
 *
 * 统计键值包含的字节数目。
 *
 * keyValue -- 指定的键值
 * 返回值，返回统计的结果。
 */
static int
GetItsSize(char const *keyValue)
{
        char const *p = NULL;
        char ch;
        int counter = 0;

        if(keyValue == NULL) //检查参数是否有效
        {
                fprintf(stdout, "传递给函数GetItsSize的参数keyValue无效。\n");
                return -1;
        }

        for(p = keyValue; (ch = *p) != '\0'; p ++) //统计键值的长度
        {
                if(ch == '&') //表示键值结束
                {
                        break;
                }
                if(ch == '%')
                {
                        p += 2;
                }
                counter ++;
        }
        return counter;
}

/*
 * 把两位16进制转换为10进制，并返回结果。
 *
 * digit1 -- 第一位16进制
 * digit2 -- 第二位16进制
 * 返回值，返回结果(返回类型是无符号整型)。
 */
static uChar
MergeThem(char digit1, char digit2)
{
        uChar result;
        int v1,v2;

        v1 = (digit1 <= 'Z' && digit1 >= 'A')? digit1 - 'A' + 10 : digit1 - '0';
        v2 = (digit2 <= 'Z' && digit2 >= 'A')? digit2 - 'A' + 10 : digit2 - '0';
        result = v1 * 16 + v2;
        return result;
}


/*
 * 外部接口
 *
 *
 * 获取指定键的键值。
 *
 * queryStr -- 请求信息的字符串
 * keyName -- 指定的键名
 * resultp -- （用指针）返回键值
 * 返回值，如果操作成功返回ESP_OK；否则返回ESP_FAIL。
 */
esp_err_t
GetTheKeyValue(char const *queryStr, char const *keyName, uChar *resultp)
{
        uChar *result = NULL; //存放键值
        char const *p = NULL;
        char ch;
        int keyValueSize;
        int i = 0;

        if(queryStr == NULL) //检查参数是否有效
        {
                fprintf(stdout, "传递给函数GetTheKeyValue的参数queryStr无效。\n");
                return ESP_FAIL;
        }
        if(keyName == NULL)
        {
                fprintf(stdout, "传递给函数GetTheKeyValue的参数keyName无效。\n");
                return ESP_FAIL;
        }
        if(resultp == NULL)
        {
                fprintf(stdout, "传递给函数GetTheKeyValue的参数resultp无效。\n");
                return ESP_FAIL;
        }

        p = strstr(queryStr, keyName); //找到键值起始位置
        if(p == NULL) //如果指定的键名不存在
        {
                resultp = result; //键值返回NULL
                return ESP_OK;
        }

        p = strchr(p, '=');
        keyValueSize = GetItsSize(++ p);
//        printf("keyValueSize=%d\r\n",keyValueSize);
        result = (uChar *)malloc((keyValueSize + 1) * sizeof(uChar)); //申请一段内存来存放键值
        if(result == NULL)
        {
                fprintf(stdout, "内存不足。\n");
                exit(EXIT_FAILURE);
        }
        while((ch = *p) != '\0' && ch != '&') //获取有效键值
        {
               if(ch == '%')
                {
                        result [i] = MergeThem(p[1], p[2]);
                        p += 3;
                }
                else
                {
                        result [i] = (uChar)ch;
                        p ++;
                }
                i ++;
        }
        result [i] = '\0'; //结尾
        memcpy(resultp,result,keyValueSize);
        free(result);
        return ESP_OK;
}



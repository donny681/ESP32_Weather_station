/*
 * http_handler.c
 *
 *  Created on: Dec 19, 2019
 *      Author: fyy
 */
#include <stdio.h>
#include <string.h>

#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "esp_netif.h"
#include "http_handler.h"
#include "my_url_parser.h"
#include "my_nvs.h"
#include "https.h"
void http_get_handler(char *url,char *data)
{
	if(!strcmp(url,data))
	{
		return ;
	}
	if (!strcmp(url,"/wifi.html"))
	{
		printf("http_get_handler\r\n");
		char buf[128]={0};
		GetTheKeyValue(data,"ssid",(unsigned char *)buf);
		printf("ssid=%s\r\n",buf);
		save_nvs_str("ssid",buf);

		memset(buf,0,128);
		read_nvs_str("ssid",buf);
		printf("read ssid=%s\r\n",buf);

		memset(buf,0,128);
		GetTheKeyValue(data,"pwd",(unsigned char *)buf);
		printf("pwd=%s\r\n",buf);
		save_nvs_str("pwd",buf);

		memset(buf,0,128);
		read_nvs_str("pwd",buf);
		printf("read pwd=%s\r\n",buf);
	}else if (!strcmp(url,"/city.html")){
		char buf[128]={0};
		GetTheKeyValue(data,"city",(unsigned char *)buf);
		printf("city=%s\r\n",buf);
		save_nvs_str("city",buf);
		memset(buf,0,128);
		read_nvs_str("city",buf);
		printf("read city=%s\r\n",buf);
		xSemaphoreGive(https_handle_t);
	}
}



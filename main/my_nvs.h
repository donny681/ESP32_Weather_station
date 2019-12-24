#ifndef MY_NVS_H_
#define MY_NVS_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
esp_err_t save_nvs_str(char *key,char *str);
esp_err_t read_nvs_str(char *key,char *str);
#endif
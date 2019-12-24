/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lvgl/lvgl.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "esp_freertos_hooks.h"


#include "disp_spi.h"
#include "ili9341.h"

#include "tp_spi.h"
#include "xpt2046.h"
#include "my_demo.h"
#include "part.h"
#include "nvs_flash.h"
#include "wifi.h"
static void IRAM_ATTR lv_tick_task(void);

/* Declare the function which starts the file server.
 * Implementation of this function is to be found in
 * file_server.c */
esp_err_t start_file_server(const char *base_path);
void app_main()
{
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
	lv_init();
	disp_spi_init();
	ili9341_init();
	spiffs_init();
#if ENABLE_TOUCH_INPUT
	tp_spi_init();
	xpt2046_init();
#endif

    static lv_color_t buf1[DISP_BUF_SIZE];
    static lv_color_t buf2[DISP_BUF_SIZE];
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf1, buf2, DISP_BUF_SIZE);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = ili9341_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

#if ENABLE_TOUCH_INPUT
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = xpt2046_read;
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);
#endif

	esp_register_freertos_tick_hook(lv_tick_task);

	my_demo_create();
	
	/*****/
	wifi_init_sta_ap();
	static int counter=0;
	/* Start the file server */
	    ESP_ERROR_CHECK(start_file_server("/spiffs"));
	while(1) {
		counter++;
		vTaskDelay(100 / portTICK_PERIOD_MS);
		lv_task_handler();
		if(!(counter%100))
		{
			counter=0;
			printf("%s buffer get: %d\r\n",__func__,  esp_get_free_heap_size());
		}

	}
}

static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
}

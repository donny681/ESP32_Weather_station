

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "lvgl/lvgl.h"
#include "esp_sntp.h"
#include "mysntp.h"
#include "my_demo.h"
#include "wifi.h"
#include "https.h"
static const char *TAG = "example";

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}


static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp1.aliyun.com");
    sntp_setservername(1, "ntp2.aliyun.com");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}


static void obtain_time(void)
{
    initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);

}


void my_sntp_task(void *arg)
{
	vTaskDelay(1000 / portTICK_PERIOD_MS);
//	 xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, true, false, portMAX_DELAY);
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        // update 'now' variable with current time
        time(&now);
    }
    if (timeinfo.tm_year < (2016 - 1900)){
        esp_restart();
    }
    char strftime_buf[64];
    // Set timezone to China Standard Time
    setenv("TZ", "CST-8", 1);
    tzset();
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
    int old_day=timeinfo.tm_min;
    while(1)
    {
        time(&now);
        localtime_r(&now, &timeinfo);
        memset(strftime_buf,0,64);
        sprintf(strftime_buf,"%02d:%02d:%02d",timeinfo.tm_hour,timeinfo.tm_min,timeinfo.tm_sec);
        lv_label_set_text(TIME_label, strftime_buf );
        memset(strftime_buf,0,64);
        sprintf(strftime_buf,"%d年%02d月%02d日",timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday);
        lv_label_set_text(Date_label, strftime_buf );//date
        memset(strftime_buf,0,64);
        sprintf(strftime_buf,"%02d/%02d",timeinfo.tm_mon+1,timeinfo.tm_mday+1);
        lv_label_set_text(Next_weather_date_label, strftime_buf );//date
        memset(strftime_buf,0,64);
        sprintf(strftime_buf,"%02d/%02d",timeinfo.tm_mon+1,timeinfo.tm_mday+2);
        lv_label_set_text(Next_two_weather_date_label, strftime_buf );//date
        memset(strftime_buf,0,64);
        sprintf(strftime_buf,"%02d/%02d",timeinfo.tm_mon+1,timeinfo.tm_mday+3);
        lv_label_set_text(Next_three_weather_date_label, strftime_buf );//date
        int new_day=timeinfo.tm_mday;
        if(new_day!=old_day)
        {
        	printf("old=%d new=%d\r\n",old_day,new_day);
        	old_day=timeinfo.tm_mday;
        	xSemaphoreGive(https_handle_t);
//        	xTaskCreate(&https_get_task, "https_get_task", 15000, NULL, 4, NULL);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

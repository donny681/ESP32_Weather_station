#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "deal_data.h"
#include "cJSON.h"
#include "lvgl/lvgl.h"
#include "my_demo.h"
const char *TAG="cJSON";
int  parse_weather_data(char *raw)
{
    cJSON *root = NULL, *code = NULL, *result = NULL,*HeWeather5=NULL,*daily_forecast=NULL,*tmp=NULL,*Array1=NULL,*Array2=NULL,*tmp_max=NULL,*tmp_min=NULL,*cond=NULL,*code_d=NULL,*now=NULL,*now_cond=NULL,*now_code=NULL,*Array3=NULL,*Array3_cond=NULL,\
    		*Array3_code_d=NULL,*Array3_tmp=NULL,*Array3_tmp_max=NULL,*Array3_tmp_min=NULL,*Array4=NULL,*Array4_cond=NULL,*Array4_code_d=NULL,*Array4_tmp=NULL,*Array4_tmp_max=NULL,*Array4_tmp_min=NULL,\
			*Array5=NULL,*Array5_cond=NULL,*Array5_code_d=NULL,*Array5_tmp=NULL,*Array5_tmp_max=NULL,*Array5_tmp_min=NULL,\
			*suggestion=NULL,*comf=NULL,*COMF_txt=NULL;
    root = cJSON_Parse(raw);
    if (!root) {
        ESP_LOGI(TAG,"JSON Parse Error");
        return -1;
    }
     code = cJSON_GetObjectItem(root, "code");
     if (code) {
         printf("code!=NULL\r\n");
         if(strcmp(code->valuestring,"10000"))
         {
             ESP_LOGI(TAG,"JSON Parse err");
             return -1;
         }
     result = cJSON_GetObjectItem(root, "result");
		 if (result) {
			 ESP_LOGI(TAG,"result");
			 HeWeather5 = cJSON_GetObjectItem(result, "HeWeather5");
			 if(HeWeather5)
			 {
				 ESP_LOGI(TAG,"HeWeather5");
				 Array1 = cJSON_GetArrayItem(HeWeather5, 0);
				 if(Array1)
				 {
					 ESP_LOGI(TAG,"Array1");
					 daily_forecast=cJSON_GetObjectItem(Array1, "daily_forecast");
					 if(daily_forecast)
					 {
						ESP_LOGI(TAG,"daily_forecast");
						Array2 = cJSON_GetArrayItem(daily_forecast, 0);
						{
							if(Array2)
							{
								ESP_LOGI(TAG,"Array2");
								tmp=cJSON_GetObjectItem(Array2, "tmp");
								if(tmp)
								{

									tmp_max=cJSON_GetObjectItem(tmp, "max");
									tmp_min=cJSON_GetObjectItem(tmp, "min");
									char buf[100]={0};
									sprintf(buf,"%s~%s",tmp_min->valuestring,tmp_max->valuestring);
									ESP_LOGI(TAG,"buf=%s",buf);
									lv_label_set_text(Next_day_tmp_label, buf);
								}
								cond=cJSON_GetObjectItem(Array2, "cond");
								if(cond){
									code_d=cJSON_GetObjectItem(cond, "code_d");
									{
										char buf[100]={0};
										sprintf(buf,"/spiffs/wet/%s.png",code_d->valuestring);
										printf("buf=%s\r\n",buf);
//										lv_img_set_src(CURRENT_img1,buf);
									}
								}
							}
						}
						Array3=cJSON_GetArrayItem(daily_forecast, 1);
						if(Array3){
							ESP_LOGI(TAG,"Array3");
							Array3_tmp=cJSON_GetObjectItem(Array3, "tmp");
							if(tmp)
							{

								Array3_tmp_max=cJSON_GetObjectItem(tmp, "max");
								Array3_tmp_min=cJSON_GetObjectItem(tmp, "min");
								char buf[100]={0};
								sprintf(buf,"%s~%s",Array3_tmp_min->valuestring,Array3_tmp_max->valuestring);
								ESP_LOGI(TAG,"buf=%s",buf);
								lv_label_set_text(Next_day_tmp_label, buf);
							}
							Array3_cond=cJSON_GetObjectItem(Array3, "cond");
							if(Array3_cond)
							{
								Array3_code_d=cJSON_GetObjectItem(Array3_cond, "code_d");
								if(Array3_code_d)
								{
									 char buf[100]={0};
									sprintf(buf,"/spiffs/wet/%s.png",Array3_code_d->valuestring);
									printf("Array3_code_d buf=%s\r\n",buf);
//									lv_img_set_src(NEXT_day_weather_img,buf);//改变当前天气图标
								}
							}
						}
						Array4=cJSON_GetArrayItem(daily_forecast, 2);
						if(Array4){
							ESP_LOGI(TAG,"Array4");

							Array4_tmp=cJSON_GetObjectItem(Array4, "tmp");
							if(Array4_tmp)
							{

								Array4_tmp_max=cJSON_GetObjectItem(Array4_tmp, "max");
								Array4_tmp_min=cJSON_GetObjectItem(Array4_tmp, "min");
								char buf[100]={0};
								sprintf(buf,"%s~%s",Array4_tmp_min->valuestring,Array4_tmp_max->valuestring);
								ESP_LOGI(TAG,"second buf=%s",buf);
								lv_label_set_text(Next_two_day_tmp_label, buf);
							}
							Array4_cond=cJSON_GetObjectItem(Array4, "cond");
							if(Array4_cond)
							{
								Array4_code_d=cJSON_GetObjectItem(Array4_cond, "code_d");
								if(Array4_code_d)
								{
									char buf[100]={0};
									sprintf(buf,"/spiffs/wet/%s.png",Array4_code_d->valuestring);
									printf("Array4_code_d buf=%s\r\n",buf);
									lv_img_set_src(NEXT_two_day_weather_img,buf);//改变当前天气图标
								}
							}
						}
						Array5=cJSON_GetArrayItem(daily_forecast, 3);
						if(Array5){
							ESP_LOGI(TAG,"Array5");
							Array5_tmp=cJSON_GetObjectItem(Array5, "tmp");
							if(Array5_tmp)
							{
								Array5_tmp_max=cJSON_GetObjectItem(Array5_tmp, "max");
								Array5_tmp_min=cJSON_GetObjectItem(Array5_tmp, "min");
								char buf[100]={0};
								sprintf(buf,"%s~%s",Array5_tmp_min->valuestring,Array5_tmp_max->valuestring);
								ESP_LOGI(TAG,"buf=%s",buf);
								lv_label_set_text(Next_three_day_tmp_label, buf);
							}
							Array5_cond=cJSON_GetObjectItem(Array5, "cond");
							if(Array5_cond)
							{
								Array5_code_d=cJSON_GetObjectItem(Array5_cond, "code_d");
								if(Array5_code_d)
								{
									 char buf[100]={0};
									sprintf(buf,"/spiffs/wet/%s.png",Array5_code_d->valuestring);
									printf("Array5_code_d buf=%s\r\n",buf);
									lv_img_set_src(NEXT_three_day_weather_img,buf);//改变当前天气图标
								}
							}
						}
					 }
					 suggestion=cJSON_GetObjectItem(Array1, "suggestion");
					 if(suggestion){
						 ESP_LOGI(TAG, "suggestion.");
						 comf=cJSON_GetObjectItem(suggestion, "comf");
						 if(comf){
							 ESP_LOGI(TAG, "comf.");
							 COMF_txt=cJSON_GetObjectItem(comf, "txt");
							 if(COMF_txt){
								 printf("COMF_brf:%s\r\n",COMF_txt->valuestring);
								 lv_label_set_text(Current_weather_tip_label, COMF_txt->valuestring );
							 }
						 }
					 }
					 now=cJSON_GetObjectItem(Array1, "now");
					 {
						 now_cond=cJSON_GetObjectItem(now, "cond");
						 if(now_cond)
						 {
							 now_code=cJSON_GetObjectItem(now_cond, "code");
							 {
								 if(now_code)
								 {
									 char buf[100]={0};
									 sprintf(buf,"/spiffs/wet/%s.png",code_d->valuestring);
									 printf("now buf=%s\r\n",buf);
									 lv_img_set_src(CURRENT_img1,buf);//改变当前天气图标
								 }
							 }
						 }
					 }
				 }
			 }
		 }


     }
     cJSON_Delete(root);
     return 0;
}

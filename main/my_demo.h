/*
 * my_demo.h
 *
 *  Created on: Dec 5, 2019
 *      Author: fyy
 */

#ifndef MY_DEMO_H_
#define MY_DEMO_H_
#include "lvgl/lvgl.h"
#define LANGUAGE 1//1.Chinese 2.English
void my_demo_create();
extern lv_obj_t * Date_label;//日期label
extern lv_obj_t * TIME_label;//时间label
extern lv_obj_t * CURRENT_img1;//当天天气图像img
extern lv_obj_t * NEXT_day_weather_img;//明天天气label
extern lv_obj_t * NEXT_two_day_weather_img;//后天天气图像img
extern lv_obj_t * NEXT_three_day_weather_img;//后两天天气图像img
extern lv_obj_t * Current_weather_label;//当前天气label
extern lv_obj_t * Next_weather_date_label;//明天日期label
extern lv_obj_t * Next_two_weather_date_label;//后天日期label
extern lv_obj_t * Next_three_weather_date_label;//后两天天气label
extern lv_obj_t * Next_day_tmp_label;//明天天气温度label
extern lv_obj_t * Next_two_day_tmp_label;//过两天天气温度label
extern lv_obj_t * Next_three_day_tmp_label;//过三天天气温度label
extern lv_obj_t * city_label;//城市label
extern lv_obj_t * Current_weather_tip_label;//当前天气提示label


#endif /* MY_DEMO_H_ */

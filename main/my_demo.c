/*
 * my_demo.c
 *
 *  Created on: Dec 5, 2019
 *      Author: fyy
 */

#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>

#include <stdlib.h>
#include "lodepng.h"
#include "esp_log.h"
#include "part.h"
#include "lv_lodepng.h"
#include "my_demo.h"
#include "mysntp.h"
#include "my_nvs.h"
#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_TEST
#define LV_ATTRIBUTE_IMG_TEST
#endif
static const char *TAG="my_demo";
lv_obj_t * Date_label= NULL;//日期label
lv_obj_t * TIME_label= NULL;//时间label
lv_obj_t * CURRENT_img1=NULL;//当天天气图像img
lv_obj_t * NEXT_day_weather_img=NULL;//明天天气图像img
lv_obj_t * NEXT_two_day_weather_img=NULL;//后天天气图像img
lv_obj_t * NEXT_three_day_weather_img=NULL;//后两天天气图像img
lv_obj_t * Current_weather_label= NULL;//当前天气温度label
lv_obj_t * Current_weather_tip_label= NULL;//当前天气提示label
lv_obj_t * Next_weather_date_label= NULL;//明天日期label
lv_obj_t * Next_two_weather_date_label= NULL;//后天日期label
lv_obj_t * Next_three_weather_date_label= NULL;//后两天天气label
lv_obj_t * Next_day_tmp_label= NULL;//明天天气温度label
lv_obj_t * Next_two_day_tmp_label= NULL;//过两天天气温度label
lv_obj_t * Next_three_day_tmp_label= NULL;//过三天天气温度label
lv_obj_t * city_label= NULL;//城市label


//void lv_img_set_src(lv_obj_t *obj,char *src)
//{
//	lv_obj_del(obj);
//}
void my_demo_create()
{
    lv_lodepng_init();
    static lv_style_t time_style;
    char *time_str="00:00:00";
    /*Create a style and use the new font*/
    lv_style_copy(&time_style, &lv_style_plain);
    time_style.text.font = &songti_simple_32; /* 设置自定义字体 */
    /*Create a label and set new text*/
    TIME_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(TIME_label, 50, 25);
    lv_label_set_align(TIME_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(TIME_label, LV_LABEL_STYLE_MAIN,&time_style);
    lv_label_set_text(TIME_label, time_str );

    /*current weather*/
    CURRENT_img1 = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_pos(CURRENT_img1, 10, 65);
    lv_img_set_src(CURRENT_img1, "/spiffs/wet/100.png");
    lv_obj_set_drag(CURRENT_img1, true);


    /**weather current label */
    static lv_style_t temperature_style;
    char *temperature_str="20~28";
    lv_style_copy(&temperature_style, &lv_style_plain);
    temperature_style.text.font = &lv_font_roboto_28; /* 设置自定义字体 */
    Current_weather_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Current_weather_label, 140, 115);
    lv_label_set_align(Current_weather_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Current_weather_label, LV_LABEL_STYLE_MAIN,&temperature_style);
    lv_label_set_text(Current_weather_label, temperature_str );

       /*divided line*/
    static  lv_point_t line_points[] = { {0, 75},{240, 75} };
    /*Create new style (thick dark blue)*/
    static lv_style_t style_line;
    lv_style_copy(&style_line, &lv_style_plain);
    style_line.line.color = LV_COLOR_MAKE(0x00, 0x3b, 0x75);
    style_line.line.width = 3;
    style_line.line.rounded = 1;
    lv_obj_t * line1;
    line1 = lv_line_create(lv_scr_act(), NULL);
    lv_line_set_points(line1, line_points, 2);     /*Set the points*/
    lv_line_set_style(line1, LV_LINE_STYLE_MAIN, &style_line);
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);

    /**Next day weather */
    NEXT_day_weather_img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_pos(NEXT_day_weather_img, 0, 185);
    lv_img_set_src(NEXT_day_weather_img, "/spiffs/wet/101.png");
    lv_obj_set_drag(NEXT_day_weather_img, true);

    static lv_style_t next_day_label_style;
    char *next_day_str="12/11";
    lv_style_copy(&next_day_label_style, &lv_style_plain);
    next_day_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_weather_date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_weather_date_label, 20, 280);
    lv_label_set_align(Next_weather_date_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_weather_date_label, LV_LABEL_STYLE_MAIN,&next_day_label_style);
    lv_label_set_text(Next_weather_date_label, next_day_str );

    static lv_style_t Next_day_tmp_label_style;
    char *next_day_tmp_str="0~1";
    lv_style_copy(&Next_day_tmp_label_style, &lv_style_plain);
    Next_day_tmp_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_day_tmp_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_day_tmp_label, 20, 300);
    lv_label_set_align(Next_day_tmp_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_day_tmp_label, LV_LABEL_STYLE_MAIN,&Next_day_tmp_label_style);
    lv_label_set_text(Next_day_tmp_label, next_day_tmp_str );

     /*after two day weather*/
    NEXT_two_day_weather_img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_pos(NEXT_two_day_weather_img, 80, 185);
    lv_img_set_src(NEXT_two_day_weather_img, "/spiffs/wet/102.png");

    static lv_style_t next_two_day_label_style;
    char *next_two_day_str="12/12";
    lv_style_copy(&next_two_day_label_style, &lv_style_plain);
    next_two_day_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_two_weather_date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_two_weather_date_label, 90, 280);
    lv_label_set_align(Next_two_weather_date_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_two_weather_date_label, LV_LABEL_STYLE_MAIN,&next_two_day_label_style);
    lv_label_set_text(Next_two_weather_date_label, next_two_day_str );

    static lv_style_t Next_two_day_tmp_label_style;
    char *Next_two_day_tmp_str="0~1";
    lv_style_copy(&Next_two_day_tmp_label_style, &lv_style_plain);
    Next_two_day_tmp_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_two_day_tmp_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_two_day_tmp_label, 90, 300);
    lv_label_set_align(Next_two_day_tmp_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_two_day_tmp_label, LV_LABEL_STYLE_MAIN,&Next_two_day_tmp_label_style);
    lv_label_set_text(Next_two_day_tmp_label, Next_two_day_tmp_str );
      /*after three day weather*/
    NEXT_three_day_weather_img = lv_img_create(lv_scr_act(), NULL);
    lv_obj_set_pos(NEXT_three_day_weather_img, 160, 185);
    lv_img_set_src(NEXT_three_day_weather_img, "/spiffs/wet/103.png");

    static lv_style_t next_three_day_label_style;
    char *next_three_day_str="12/13";
    lv_style_copy(&next_three_day_label_style, &lv_style_plain);
    next_three_day_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_three_weather_date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_three_weather_date_label, 175, 280);
    lv_label_set_align(Next_three_weather_date_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_three_weather_date_label, LV_LABEL_STYLE_MAIN,&next_three_day_label_style);
    lv_label_set_text(Next_three_weather_date_label, next_three_day_str );

    static lv_style_t Next_three_day_tmp_label_style;
    char *Next_three_day_tmp_str="0~1";
    lv_style_copy(&Next_three_day_tmp_label_style, &lv_style_plain);
    Next_three_day_tmp_label_style.text.font = &lv_font_roboto_16; /* 设置自定义字体 */
    Next_three_day_tmp_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Next_three_day_tmp_label, 175, 300);
    lv_label_set_align(Next_three_day_tmp_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Next_three_day_tmp_label, LV_LABEL_STYLE_MAIN,&Next_three_day_tmp_label_style);
    lv_label_set_text(Next_three_day_tmp_label, Next_three_day_tmp_str );
#if LANGUAGE==1
    /*city name label*/
    static lv_style_t city_style;

    char city_str[64]={0};
    read_nvs_str("city",city_str);
    /*Create a style and use the new font*/
    lv_style_copy(&city_style, &lv_style_plain);
    city_style.text.font = &songti_22; /* 设置自定义字体 */
    /*Create a label and set new text*/
    city_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(city_label, 140, 80);
    lv_label_set_align(city_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(city_label, LV_LABEL_STYLE_MAIN,&city_style);
    lv_label_set_text(city_label, city_str );

     static lv_style_t date_style;
    char *date_str="2019年01月01号";
    lv_style_copy(&date_style, &lv_style_plain);
    date_style.text.font = &songti_16; /* 设置自定义字体 */
    Date_label = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(Date_label, 30, 5);
    lv_label_set_align(Date_label, LV_LABEL_ALIGN_CENTER);
    lv_label_set_style(Date_label, LV_LABEL_STYLE_MAIN,&date_style);
    lv_label_set_text(Date_label, date_str );


        /**weather tip current label */
     static lv_style_t current_tip_style;
     char *current_tip_str="适宜洗车，未来持续两天无雨天气较好，适合擦洗汽车，蓝天白云、风和日丽将伴您的车子连日洁净";
     lv_style_copy(&current_tip_style, &lv_style_plain);
     current_tip_style.text.font = &songti_16; /* 设置自定义字体 */
     Current_weather_tip_label = lv_label_create(lv_scr_act(), NULL);
     lv_obj_set_pos(Current_weather_tip_label, 10, 160);
     lv_label_set_align(Current_weather_tip_label, LV_LABEL_ALIGN_CENTER);
     lv_label_set_style(Current_weather_tip_label, LV_LABEL_STYLE_MAIN,&current_tip_style);
     lv_label_set_anim_speed(Current_weather_tip_label,25);
     lv_label_set_long_mode(Current_weather_tip_label,LV_LABEL_LONG_SROLL);
     lv_obj_set_size(Current_weather_tip_label,200,30);
     lv_label_set_text(Current_weather_tip_label, current_tip_str );
#endif
}


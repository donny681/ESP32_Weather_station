#include "part.h"

#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
// #include "myblock.h"
#include "lodepng.h"
#include "lvgl/lvgl.h"
static const char *TAG = "example";
int file_size(char* name)
{
    // Open for reading hello.txt
    FILE* f = fopen(name, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open time.png");
        return;
    }
    int length=0;
    long cur_pos;
    long len;
    //取得当前文件流的读取位置
    cur_pos = ftell( f );
    //将文件流的读取位置设为文件末尾
    fseek( f, 0, SEEK_END );
    //获取文件末尾的读取位置,即文件大小
    len = ftell( f );
    //将文件流的读取位置还原为原先的值
    fseek( f, cur_pos, SEEK_SET );
    fclose(f);
     printf("length=%ld\r\n",len-cur_pos);
    return len-cur_pos;
}


void decodeTwoSteps(const char* filename)
{
  unsigned error;
  unsigned char* image;
  unsigned width, height;
  unsigned char* png = 0;
  size_t pngsize;
  
  struct timespec start, stop;
  double dif;
  
  error = lodepng_load_file(&png, &pngsize, filename);
  if(error) { 
      printf("error %u: %s\n", error, lodepng_error_text(error)); 
        return;
  }

//   clock_gettime(CLOCK_MONOTONIC, &start);
        
  if(!error) {
     error = lodepng_decode32(&image, &width, &height, png, pngsize);
 }
  if(error) {
      printf("error %u: %s\n", error, lodepng_error_text(error));
      return ;
  }
  
//   clock_gettime(CLOCK_MONOTONIC, &stop);
//   dif = (stop.tv_sec - start.tv_sec) * 1e6 + (stop.tv_nsec - start.tv_nsec) / 1e3;    // in microseconds
//   printf("dif: %.4f\n", dif);
    printf("width=%d,height=%d\r\n", width,height);

/********************************************************/
    /*Initialize an image descriptor for LittlevGL with the decoded image*/
    lv_img_dsc_t png_dsc;
    png_dsc.header.always_zero = 0;                          /*It must be zero*/
    png_dsc.header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;      /*Set the color format*/
    png_dsc.header.w = width;
    png_dsc.header.h = height;
    png_dsc.data_size = width * height * 4;
    png_dsc.data = image;

    /*Create an image object and set the decoded PNG image as it's source*/
    lv_obj_t * img_obj = lv_img_create(lv_scr_act(), NULL);     /*Create the an image object in LittlevGL*/
    lv_img_set_src(img_obj, &png_dsc);                          /*Set the image source to the decoded PNG*/
    lv_obj_set_pos(img_obj, 0, 0);
    // lv_obj_set_drag(img_obj, true);                             /*Make to image dragable*/


	// imgBackground = lv_img_create(scr, NULL);
	// lv_img_set_src(imgBackground, &png_dsc);
	// lv_obj_set_size(imgBackground, LV_HOR_RES, LV_VER_RES);
	// lv_obj_set_pos(imgBackground, 0, 0);

    /*Set a non-white background color for the screen to see the alpha is working on the image*/
    // static lv_style_t new_style;
    // lv_style_copy(&new_style, lv_style_scr);
    // new_style.body.main_color = LV_COLOR_MAKE(0x40, 0x70, 0xAA);
    // lv_obj_set_style(lv_scr_act(), &new_style);

/********************************************************/

#ifdef MEM_BLOCK  
  FreeMem(png);
#else  
  free(png);
#endif  


  /*use image here*/
#ifdef MEM_BLOCK  
  FreeMem(image);
#else  
  free(image);
#endif
}


void spiffs_init()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");
    
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    
    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    // ESP_LOGI(TAG, "Reading time.png");
    printf("%d\r\n",file_size("/spiffs/time.png"));    
}

void show_picture()
{
    

}

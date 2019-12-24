#include "my_nvs.h"
esp_err_t save_nvs_str(char *key,char *str)
{
    nvs_handle_t my_handle;
    esp_err_t err;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ESP_FAIL;
    } 
        printf("Done\n");
        err =nvs_set_str(my_handle,key,str);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        err = nvs_commit(my_handle);
        printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
        // Close
        nvs_close(my_handle);
    return ESP_OK;
}

esp_err_t read_nvs_str(char *key,char *str){
    nvs_handle_t my_handle;
    size_t length=64;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ESP_FAIL;
    } 
        printf("open Done\n");
        err =nvs_get_str(my_handle,key,str,&length);
        switch (err) {
            case ESP_OK:
                printf("length = %d,%s\n", length,str);
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
                break;
            default :
                printf("Error (%s) reading!\n", esp_err_to_name(err));
        }
        nvs_close(my_handle);
//        vTaskDelay(1000 / portTICK_PERIOD_MS);
    return ESP_OK;
}

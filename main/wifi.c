#include "wifi.h"
#include "mysntp.h"
#include "https.h"
#include "my_nvs.h"
static const char *TAG="WIFI";

/* FreeRTOS event group to signal when we are connected*/
EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about one event 
 * - are we connected to the AP with an IP? */
const int WIFI_CONNECTED_BIT = BIT0;
#define EXAMPLE_ESP_MAXIMUM_RETRY 50

#define AP_SSID "esp32_station"
#define AP_PWD "wifiwifi"
//static int s_retry_num = 0;
static void event_handler(void* arg, esp_event_base_t event_base, 
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
           esp_wifi_connect();
           xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        //     s_retry_num++;
        //     ESP_LOGI(TAG, "retry to connect to the AP");
        // }
        // ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
//        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        xTaskCreate(&my_sntp_task, "my_sntp_task", 15000, NULL, 5, NULL);
        xTaskCreate(&https_get_task, "https_get_task", 15000, NULL, 4, NULL);
    }else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_sta_ap(void)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));


    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    wifi_config_t wifi_config_sta = {
            .sta = {
                .ssid = "Netcore",
                .password = "wifiwifi"
          },
     };
    char ssid_buf[32]={0};
    read_nvs_str("ssid",ssid_buf);
    memset(wifi_config_sta.sta.ssid,0,sizeof(wifi_config_sta.sta.ssid));
    strcpy((char *)wifi_config_sta.sta.ssid,ssid_buf);
    memset(wifi_config_sta.sta.password,0,sizeof(wifi_config_sta.sta.password));
    char pwd_buf[64]={0};
    memset(pwd_buf,0,sizeof(pwd_buf));
    read_nvs_str("pwd",pwd_buf);

    strcpy((char *)wifi_config_sta.sta.password,pwd_buf);
    wifi_config_t wifi_config_ap ={
    	.ap = {
    		.ssid = AP_SSID,
    		.ssid_len = strlen(AP_SSID),
    		.password = AP_PWD,
    		.max_connection = 5,
    		.authmode = WIFI_AUTH_WPA_WPA2_PSK
    	},

    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config_sta) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s",
    		wifi_config_sta.sta.ssid, wifi_config_sta.sta.password);
//    heap_caps_log_heap_info(MALLOC_CAP_SPIRAM);
}

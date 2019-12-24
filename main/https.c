
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

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "my_nvs.h"
#include "https.h"
#include "deal_data.h"
#include "http_parser.h"
#include "wifi.h"
#include "my_demo.h"
// #include "url_parser.h"
/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "way.jd.com"
#define WEB_PORT "443"
#define WEB_URL "/he/freeweather?city=beijing&appkey=fbca1843f9dc05f5b4b572c2c5632f00"
#define KEY ""
static const char *TAG = "example";

/* Root cert for howsmyssl.com, taken from server_root_cert.pem

   The PEM file was extracted from the output of this command:
   openssl s_client -showcerts -connect www.howsmyssl.com:443 </dev/null

   The CA root cert is the last cert given in the chain of certs.

   To embed it in the app binary, the PEM file is named
   in the component.mk COMPONENT_EMBED_TXTFILES variable.
*/
SemaphoreHandle_t  https_handle_t=NULL;
extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");
char *GLOBAL_buffer=NULL;
BUFFER_t buffer={
    .length=0,
    .request_flag=0,
    .position=0,
//    .buf=NULL
};
void init_buffer()
{
	buffer.length=0;
	buffer.position=0;
    buffer.request_flag=0;
}

void free_buffer()
{
	free(GLOBAL_buffer);
	GLOBAL_buffer=NULL;
}
static int header_value_callback(http_parser* a, const char *at, size_t length){
	return 0;
}
static int body_callback(http_parser* a, const char *at, size_t length){
    // printf("%s\r\n",at);
    buffer.length+=length;
//    if(buffer.buf==NULL)
    if(GLOBAL_buffer==NULL)
    {
        printf("malloc length=%d,buffer.length=%d\r\n",length,buffer.length);
//        buffer.buf=(char *)malloc(length+1);
        GLOBAL_buffer=(char *)malloc(length+1);
//        memset(buffer.buf,0,length+1);
//        strncpy(buffer.buf,at,length);
        memset(GLOBAL_buffer,0,length+1);
        strncpy(GLOBAL_buffer,at,length);
    }else{
        printf("realloc length=%d,buffer.length=%d\r\n",length,buffer.length);
//         buffer.buf=(char *)realloc(buffer.buf,buffer.length+1);
//         strncat(buffer.buf,at,length);
        GLOBAL_buffer=(char *)realloc(GLOBAL_buffer,buffer.length+1);
        strncat(GLOBAL_buffer,at,length);
    }
    GLOBAL_buffer[buffer.length]='\0';
    return 0;
}
static int body_done_callback (http_parser* a){
  	ESP_LOGI(TAG,"body done");
      buffer.request_flag=1;
    return 0;
}
static int begin_callback (http_parser* a){
  	ESP_LOGI(TAG,"message begin");
    return 0;
}
static int header_complete_callback(http_parser* a){
	ESP_LOGI(TAG,"header completed");
    return 0;
}

static http_parser_settings settings =
{   .on_message_begin = begin_callback
    ,.on_header_field = 0
    ,.on_header_value = header_value_callback
    ,.on_url = 0
    ,.on_status = 0
    ,.on_body = body_callback
    ,.on_headers_complete = header_complete_callback
    ,.on_message_complete = body_done_callback
    ,.on_chunk_header = 0
    ,.on_chunk_complete = 0
};



void https_get_task(void *pvParameters)
{
	https_handle_t = xSemaphoreCreateBinary();
	vTaskDelay(1000 / portTICK_PERIOD_MS);
//	 xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, true, false, portMAX_DELAY);
    // save_nvs_str("city","江门");
    init_buffer();
    
    char buf[512];
    int ret, flags, len;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(&conf);

    mbedtls_entropy_init(&entropy);
    if((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
//        goto exit;
    }

    ESP_LOGI(TAG, "Loading the CA root certificate...");

    ret = mbedtls_x509_crt_parse(&cacert, server_root_cert_pem_start,
                                 server_root_cert_pem_end-server_root_cert_pem_start);

    if(ret < 0)
    {
        ESP_LOGE(TAG, "mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        abort();
//        goto exit;
    }

    ESP_LOGI(TAG, "Setting hostname for TLS session...");

     /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&ssl, WEB_SERVER)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
//        goto exit;
    }

    ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

    if((ret = mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        goto exit;
    }

    /* MBEDTLS_SSL_VERIFY_OPTIONAL is bad for security, in this example it will print
       a warning if CA verification fails but it will continue to connect.

       You should consider using MBEDTLS_SSL_VERIFY_REQUIRED in your own code.
    */
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x\n\n", -ret);
        goto exit;
    }
/**/
    /* parse response */
    http_parser parser;
    http_parser_init(&parser, HTTP_RESPONSE);
    // parser.data = user_data;
     esp_err_t nparsed = 0;
     xSemaphoreGive(https_handle_t);
     while(1) {
    	 xSemaphoreTake(https_handle_t,portMAX_DELAY);
    	 char city[12]={0};
    	    read_nvs_str("city",city);
    	    lv_label_set_text(city_label, city );
    	    char REQUEST[512]={0};
    	    strcpy(REQUEST,"GET /he/freeweather?city=");
    	    strcat(REQUEST,city);
    	    strcat(REQUEST,"&appkey=");
    	    strcat(REQUEST,KEY);
    	    strcat(REQUEST," HTTP/1.1\r\n""Host: "WEB_SERVER"\r\n""User-Agent: esp-idf/1.0 esp32\r\n""\r\n");
    	    printf("request:\r\n%s\r\n",REQUEST);

        mbedtls_net_init(&server_fd);


       
        ESP_LOGI(TAG, "Connecting to %s:%s...", WEB_SERVER, WEB_PORT);

        if ((ret = mbedtls_net_connect(&server_fd, WEB_SERVER,
                                      WEB_PORT, MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
            goto exit;
        }

        ESP_LOGI(TAG, "Connected.");

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
                goto exit;
            }
        }

        ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

        if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
        {
            /* In real life, we probably want to close connection if ret != 0 */
            ESP_LOGW(TAG, "Failed to verify peer certificate!");
            bzero(buf, sizeof(buf));
            mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
            ESP_LOGW(TAG, "verification info: %s", buf);
        }
        else {
            ESP_LOGI(TAG, "Certificate verified.");
        }

        ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));

        ESP_LOGI(TAG, "Writing HTTP request...");

        size_t written_bytes = 0;
        do {
            ret = mbedtls_ssl_write(&ssl,
                                    (const unsigned char *)REQUEST + written_bytes,
                                    strlen(REQUEST) - written_bytes);
            if (ret >= 0) {
                ESP_LOGI(TAG, "%d bytes written", ret);
                written_bytes += ret;
            } else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
                ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
                goto exit;
            }
        } while(written_bytes < strlen(REQUEST));

        ESP_LOGI(TAG, "Reading HTTP response...");

        do
        {
            len = sizeof(buf) - 1;
            bzero(buf, sizeof(buf));
            printf("before mbedtls_ssl_read 1 packet\r\n");
            if(buffer.request_flag==1){
                ret=0;
                break;
            }
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);
            printf("mbedtls_ssl_read 1 packet\r\n");
            if(ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
                continue;

            if(ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = 0;
                break;
            }

            if(ret < 0)
            {
                ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }

            if((ret == 0)||(buffer.request_flag==1))
            {
                ESP_LOGI(TAG, "connection closed");
                break;
            }

            len = ret;
            ESP_LOGD(TAG, "%d bytes read", len);
            nparsed = http_parser_execute(&parser, &settings, buf, len);
        } while(1);

        mbedtls_ssl_close_notify(&ssl);
        printf("buf=%s\r\n",GLOBAL_buffer);
        parse_weather_data(GLOBAL_buffer);
    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);
        if(ret != 0)
        {
            mbedtls_strerror(ret, buf, 100);
            ESP_LOGE(TAG, "Last error was: -0x%x - %s", -ret, buf);
        }
        static int request_count;
        ESP_LOGI(TAG, "Completed %d requests", ++request_count);
        init_buffer();
        free_buffer();
//        vTaskDelay(5000 / portTICK_PERIOD_MS);
     }
        vTaskDelete(NULL);
}

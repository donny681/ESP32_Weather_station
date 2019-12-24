#ifndef HTTPS_H_
#define HTTPS_H_

typedef struct BUFFER_T{
    int length;
    int request_flag;
    int position;
}BUFFER_t;
void https_get_task(void *pvParameters);
extern SemaphoreHandle_t  https_handle_t;

#endif

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "bleManager.h" 
#include "alarmManager.h"

AlarmInfo alarmInfo =  {NONE, 0};

void vAlarmReceiveTask(void* params){
    // lokal "behållare" för värdet som tas emot från kön.
    int val;

    for (;;){
        // vänta här - tills något finns i kön.. -> (kopigera då in värdet i 'val')
        if (xQueueReceive(alarmQueue, &val, portMAX_DELAY)){
            ESP_LOGI("ALARM", "Värde mottaget: %d", val);
        }
    }
}


//void alarmSendState(){
    // skickar till arudino: 0,1,2 (disarmed, armed-away, armed-home)

//}
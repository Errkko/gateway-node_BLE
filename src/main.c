#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"             // För xQueueSend / xQueueReceive
#include "gpio.h"
#include "ble_manager.h"
#include "alarm.h"
#include "disp_config.h"
#include "disp_ui.h"
#include "rfid.h"

//QueueHandle_t sensor_queue = NULL;
SemaphoreHandle_t xAlarmSemaphore;
SemaphoreHandle_t xGuiSemaphore; 
QueueHandle_t alarmQueue;
QueueHandle_t stateQueue;
QueueHandle_t doorQueue;
TimerHandle_t xEntryDelayTimer;

void app_main() {

    // skapa larm kö
    alarmQueue = xQueueCreate(10, sizeof(AlarmInfo));
    stateQueue = xQueueCreate(1, sizeof(uint8_t)); 
    doorQueue = xQueueCreate(1, sizeof(uint8_t)); 
    xAlarmSemaphore = xSemaphoreCreateBinary();
    xGuiSemaphore = xSemaphoreCreateMutex();
    xEntryDelayTimer = xTimerCreate("EntryTimer", pdMS_TO_TICKS(ENTRY_TIMER_COUNTDOWN), pdFALSE, (void*)0, vEntryDelayCallback);

    disp_HW_init();
    disp_UI_init();
    initBLE();
    //initRFID();

    // Skapa RTOS tasks..
    xTaskCreatePinnedToCore(vBLETask, "BLE_HOST", 8192, NULL, 3, NULL, 0);            // CORE 0: För BLE task (ska ej ha 'TaskDelay')
    xTaskCreatePinnedToCore(vReceiveDataTask, "Rx_DATA", 4096, NULL, 4, NULL, 0);     // CORE 0: Tar emot larmpaket samt remote kommando (BLE)
    xTaskCreatePinnedToCore(vTransmitDataTask, "Tx_DATA", 4000, NULL, 4, NULL, 0);    // CORE 0: Skickar larmstatus & heartbeat (BLE)
    xTaskCreatePinnedToCore(vAlarmManagerTask, "ALARM", 4096, NULL, 5, NULL, 1);      // CORE 1: För larmlogik (OBS: ej på H2, har bara en kärna)

    xTaskCreate(lvgl_port_task, "LVGL", 1024 * 16, NULL, 2, NULL);
    //xTaskCreate(sensor_read, "DHT11_Task", 1024 * 4, NULL, 2, NULL);
    
    vTaskDelete(NULL);
}

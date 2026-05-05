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

void runtime_stats(){
    
    vTaskDelay(pdMS_TO_TICKS(10000)); 

    static char buffer[1024];

    while (1) {
        printf("\n==========================================\n");
        printf("--- SYSTEM RUNTIME STATS ---\n");
        
        // Hämta stats, glöm inte att fixa inställningar menuconfig
        vTaskGetRunTimeStats(buffer);
        printf("%s", buffer);
        
        printf("------------------------------------------\n");
        printf("Free Heap: %d bytes\n", esp_get_free_heap_size());
        printf("Min Free Heap: %d bytes\n", esp_get_minimum_free_heap_size());
        printf("==========================================\n");

        // Körs bara var 10:e sekund för att inte skräpa ner i loggen
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_main() {

    // skapa larm kö
    alarmQueue = xQueueCreate(10, sizeof(AlarmInfo));
    stateQueue = xQueueCreate(1, sizeof(uint8_t)); // ----------> not finish yet
    xAlarmSemaphore = xSemaphoreCreateBinary();
    xGuiSemaphore = xSemaphoreCreateMutex();
    //sensor_queue = xQueueCreate(5, sizeof(sensor_data_t)); // Om du har en sensor-kö

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
    xTaskCreatePinnedToCore(runtime_stats, "MonitorTask", 4096, NULL, 1, NULL, 1);
    
    vTaskDelete(NULL);
}

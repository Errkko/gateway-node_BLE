#include "dht11_driver.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"
#include "esp_cpu.h"

int read_dht11(gpio_num_t pin, float *temp, float *hum) {
    uint8_t data[5] = {0};
    uint32_t durations[40] = {0};

    gpio_set_direction(pin, GPIO_MODE_OUTPUT_OD); 
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
    
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(20)); 
    gpio_set_level(pin, 1);
    ets_delay_us(40);
    gpio_set_direction(pin, GPIO_MODE_INPUT);

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);

    // Handshake
    uint16_t t = 0;
    while(gpio_get_level(pin) == 1) { if(t++ > 200) { portEXIT_CRITICAL(&mux); return 1; } ets_delay_us(1); }
    t = 0;
    while(gpio_get_level(pin) == 0) { if(t++ > 200) { portEXIT_CRITICAL(&mux); return 1; } ets_delay_us(1); }
    t = 0;
    while(gpio_get_level(pin) == 1) { if(t++ > 200) { portEXIT_CRITICAL(&mux); return 1; } ets_delay_us(1); }

    // Read bits
    for (int i = 0; i < 40; i++) {
        while (gpio_get_level(pin) == 0); // Wait for LOW to end
        
        uint32_t start = esp_cpu_get_cycle_count();
        while (gpio_get_level(pin) == 1); // Time the HIGH
        durations[i] = esp_cpu_get_cycle_count() - start;

        if (durations[i] > 3500) {
            data[i / 8] |= (1 << (7 - (i % 8)));
        }
    }
    
    portEXIT_CRITICAL(&mux);

    if (data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
        if (data[0] == 0 && data[2] == 0) return 4; // Check for "All Zeros"
        *hum = data[0];  
        *temp = data[2];
        return 0;
    }
    return 3;
}

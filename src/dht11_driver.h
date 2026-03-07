#ifndef DHT11_DRIVER_H
#define DHT11_DRIVER_H

#include <stdbool.h>
#include "driver/gpio.h"

int read_dht11(gpio_num_t pin, float *temp, float *hum);

#endif


//
// Created by chao on 25/10/2021.
//
//
// Created by chao on 25/10/2021.
//
#include <stdio.h>
#include "hardware_setup.h"
#include "sensor_lib/sht31.h"
#include "pico/stdlib.h"

int main(void)
{
    float temp, humidity;

    initializePeripherals();

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("Testing the SHT31 now\r\n");
            printf("=====================================================\r\n");
            printf("It first checks if SHT31 is available.\r\n");
            printf("It tries to read the room temperature and humidity.\r\n");
            printf("=====================================================\r\n");

            setup_sht31();
            sht31_read_temp_hum(&temp, &humidity);
            printf("temp: %f, humidity: %f\r\n", temp, humidity);
        }
    }
}

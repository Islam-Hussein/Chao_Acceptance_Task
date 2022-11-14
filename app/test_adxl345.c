//
// Created by chao on 25/10/2021.
//
//
// Created by chao on 25/10/2021.
//
#include <stdio.h>
#include "hardware_setup.h"
#include "sensor_lib/adxl345.h"
#include "pico/stdlib.h"

int main(void)
{
    int16_t xAccl, yAccl, zAccl;

    initializePeripherals();

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("Testing the ADXL345 now\r\n");
            printf("=====================================================\r\n");
            printf("It first checks if ADXL345 is available.\r\n");
            printf("It tries to read the acceleration data from the sensor.\r\n");
            printf("=====================================================\r\n");

            setup_adxl345();
            adxl345_readData(&xAccl, &yAccl, &zAccl);
            printf("acc_x: %d, acc_y:%d, acc_z:%d\r\n", xAccl, yAccl, zAccl);
        }
    }
}

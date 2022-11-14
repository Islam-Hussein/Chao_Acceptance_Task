//
// Created by chao on 25/10/2021.
//
#include <stdio.h>
#include "hardware_setup.h"
#include "sensor_lib/pac193x.h"
#include "pico/stdlib.h"

int main(void)
{

    initializePeripherals();

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("Testing the PAC193x now\r\n");
            printf("=====================================================\r\n");
            printf("Product ID can be: 0x58, or 0x59, or 0x5a, or 0x5b\r\n");
            printf("Manufacture ID should be: 0x5d\r\n");
            printf("Revision ID might be: 0x03\r\n");
            printf("=====================================================\r\n");

            setup_pac193x();
            sleep_ms(10);
            pac193x_print_info();

        }
    }
}

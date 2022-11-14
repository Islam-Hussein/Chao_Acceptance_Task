/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// 0
// 1       @
// 2
// 3             @
// 4
// 5
// 6
// 7
//

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    // power on the PAC193x, this is very important
    gpio_init(29);
    gpio_set_dir(29, GPIO_OUT);
    gpio_put(29, 1);


    // This example will use I2C0 on the default SDA and SCL pins (0, 1 on a Pico)
    i2c_init(i2c1, 10 * 1000);
    gpio_set_function(6, GPIO_FUNC_I2C);
    gpio_set_function(7, GPIO_FUNC_I2C);
    gpio_pull_up(6);
    gpio_pull_up(7);

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("\nI2C Bus Scan\n");
            printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

            for (int addr = 0; addr < (1 << 7); ++addr) {
                if (addr % 16 == 0) {
                    printf("%02x ", addr);
                }

                // Perform a 1-byte dummy read from the probe address. If a slave
                // acknowledges this address, the function returns the number of bytes
                // transferred. If the address byte is ignored, the function returns
                // -1.

                int ret;
                uint8_t rxdata;
                ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);

                printf(ret < 0 ? "." : "@");
                printf(addr % 16 == 15 ? "\n" : "  ");
            }
            printf("Done.\n");
        }
    }
}
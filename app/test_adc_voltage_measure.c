#include <stdio.h>
#include "hardware_setup.h"
#include "pico/stdlib.h"

int main(void)
{
    initializePeripherals();

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("Will measure a voltage provided by the adjustable resistor:\r\n");

            setup_adc();

            float res_voltage = adc_measure_voltage();
            printf("Voltage: %f V\n", res_voltage);
        }
    }
}


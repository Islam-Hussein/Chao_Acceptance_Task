#include <stdio.h>
#include "hardware_setup.h"
#include "sensor_lib/adxl345.h"
#include "sensor_lib/pac193x.h"
#include "sensor_lib/sht31.h"
#include "pico/stdlib.h"

int main(void)
{
    float temp, humidity;
    int16_t xAccl, yAccl, zAccl;

    initializePeripherals();

    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            printf("Will test all sensors now:\r\n");
            setup_pac193x();
            sleep_ms(10);
            pac193x_print_info();

            setup_sht31();
            sht31_read_temp_hum(&temp, &humidity);
            printf("temp: %f, humidity: %f\r\n", temp, humidity);

            setup_adxl345();
            adxl345_readData(&xAccl, &yAccl, &zAccl);
            printf("acc_x: %d, acc_y:%d, acc_z:%d\r\n", xAccl, yAccl, zAccl);

            setup_adc();

            float res_voltage = adc_measure_voltage();
            printf("Voltage: %f V\n", res_voltage);

            adc_print_audio_record();
        }
    }
}


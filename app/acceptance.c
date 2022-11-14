/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define configUSE_Test  0

#if configUSE_Test == 0

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "sensor_lib/acceptance_task.h"



uint8_t capture_buf[SAMPLING_BUFFER_SIZE];

uint8_t *sample_address_pointer = &capture_buf[0];

static uint8_t Global_cnt = 0;



int main()
{
    int16_t xAccl, yAccl, zAccl;

    stdio_init_all();
    printf("ADC Example, measuring GPIO26\n");

    ADC_Set_SamplingRate();

    ADC_Input_Init();

    setup_i2c0();
    setup_i2c1();


    while (1)
    {


        char c = getchar_timeout_us(10000);
        if (c == 'k')
           {

            ADC_Set_SamplingRate();

            ADC_Input_Init();

            printf("ADC Values:-\r\n");

            ADC_Sample_Capture();


             
            printf("ADC Values:-\r\n");
            for (int i = 0; i < SAMPLING_BUFFER_SIZE; i++)
            {
                printf(" %d \n", capture_buf[i]);
            }
        }

        
        if (c == 'g')
        {
            uint8_t no_of_samples = 0;
            uint8_t FIFO_ADDR = 0x39;

            printf("=====================================================\r\n");
            printf("We send data for 1 Second that means 100 sample for frequency 100Hz.\r\n");
            printf("It tries to read the acceleration data from the sensor.\r\n");
            printf("=====================================================\r\n");

            ADXLl345_Init();
            while(Global_cnt < 100)
            {
                
                    i2c_write_blocking(i2c0, ADXL345_SLAVE_ADDR, &FIFO_ADDR, 1, false);
                    i2c_read_blocking(i2c0, ADXL345_SLAVE_ADDR, &no_of_samples, 1, false);

                    no_of_samples &= 0x3F;

                    if(no_of_samples == 32)
                    {
                            adxl345_ReadData(&xAccl, &yAccl, &zAccl);    
                            printf("acc_x: %d, acc_y:%d, acc_z:%d   ,cnt:%d\r\n", xAccl, yAccl, zAccl,Global_cnt);
                            Global_cnt++;            
                    }
                    else
                    {
                        //printf("FIFO is Not full\r\n");
                    }
                
            }
            if(Global_cnt >= 100 )
            {
                Global_cnt = 0;
            }



        }
    }
}

#endif

#if configUSE_Test == 1
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "pico/binary_info.h"


#define ADC_NUM 0
#define ADC_PIN (26 + ADC_NUM)
#define ADC_VREF 3.3
#define ADC_RANGE (1 << 12)
#define ADC_CONVERT (ADC_VREF / (ADC_RANGE - 1))

int main() {
    stdio_init_all();
    printf("Beep boop, listening...\n");

    bi_decl(bi_program_description("Analog microphone example for Raspberry Pi Pico")); // for picotool
    bi_decl(bi_1pin_with_name(ADC_PIN, "ADC input pin"));

    adc_init();
    adc_gpio_init( ADC_PIN);
    adc_select_input( ADC_NUM);

    uint adc_raw;
    while (1) {
        adc_raw = adc_read(); // raw voltage from ADC
        printf("%.2f\n", adc_raw * ADC_CONVERT);
        sleep_ms(10);
    }

    return 0;
}

#endif

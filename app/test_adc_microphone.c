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
            printf("This test will capture an audio signal from the environment:\r\n");
            setup_adc();

            adc_print_audio_record();
        }
    }
}


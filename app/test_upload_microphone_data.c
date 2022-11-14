#include <stdio.h>
#include "hardware_setup.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"


#define window_duration 1000.0 // in milisecond
#define SAMPLING_RATE 16000
#define NSAMP (uint32_t)(SAMPLING_RATE*window_duration/1000)

void adc_setup_for_mic(void)
{
	adc_init();

	adc_gpio_init(26);

	uint32_t adc_sampling_rate = SAMPLING_RATE;
	setup_adc_sampling_rate(adc_sampling_rate);
}


void adc_upload_audio_record(void)
{
	uint16_t adc_buf[NSAMP];

	adc_run(false);
	adc_select_input(0);
	adc_capture(adc_buf, NSAMP);
	for (int ii = 0; ii < NSAMP; ii++)
	{
		printf("%d,", adc_buf[ii]);
	}
	printf("\n");
	adc_run(false);
}

int main(void)
{
    initializePeripherals();
    adc_setup_for_mic();
    while (1)
    {
        char c = getchar_timeout_us(10000);
        if (c == 't')
        {
            adc_upload_audio_record();
        }
    }
}


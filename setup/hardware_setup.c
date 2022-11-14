#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "hardware_setup.h"
#include "sensor_lib/adxl345.h"
#include "sensor_lib/pac193x.h"
#include "sensor_lib/sht31.h"

void initializePeripherals(void)
{
	stdio_init_all();

	setup_i2c1();

	setup_i2c0();

}

void setup_pac193x(void){
    int ret=1; // be optimistic
    ret = pac193x_init(I2C_FOR_PAC1934);
    if(ret<0){
        printf("Error, the sensor PAC193x is not responding.\r\n");
    }
}

void setup_sht31(void){
    int ret=1; // be optimistic
    ret = sht31_init(I2C_FOR_SHT31);
    if(ret<0){
        printf("Error, the sensor SHT31 is not responding.\r\n");
    }
}

void setup_adxl345(void){
    int ret=1; // be optimistic
    ret = adxl345_init(I2C_FOR_ADXL345);
    if(ret<0){
        printf("Error, the sensor ADXL345 is not responding.\r\n");
    }
}


void setup_i2c1_sda_line(void)
{
	gpio_set_function(6, GPIO_FUNC_I2C);
	gpio_pull_up(6);
}

void setup_i2c1_scl_line(void)
{
	gpio_set_function(7, GPIO_FUNC_I2C);
	gpio_pull_up(7);
}

void setup_i2c1(void)
{
	int baud_rate = 100 * 1000;
	i2c_init(i2c1, baud_rate);
	setup_i2c1_sda_line();
	setup_i2c1_scl_line();
}

void setup_i2c0_sda_line(void)
{
	gpio_set_function(0, GPIO_FUNC_I2C);
	gpio_pull_up(0);
}

void setup_i2c0_scl_line(void)
{
	gpio_set_function(1, GPIO_FUNC_I2C);
	gpio_pull_up(1);
}

void setup_i2c0(void)
{
	int baud_rate = 100 * 1000;
	i2c_init(i2c0, baud_rate);
	setup_i2c0_sda_line();
	setup_i2c0_scl_line();
}

void setup_adc_sampling_rate(uint32_t sampling_rate)
{
	// When the clock of the MCU is 48MHz
	uint32_t clock_div = 48000000 / sampling_rate;
	adc_set_clkdiv(clock_div);
}

void setup_adc(void)
{
	adc_init();

	// Make sure GPIO is high-impedance, no pullups etc
	adc_gpio_init(26);
	adc_gpio_init(27);

	uint32_t adc_sampling_rate = 5000;
	setup_adc_sampling_rate(adc_sampling_rate);
}

float adc_measure_voltage(void)
{
	// 12-bit conversion, assume max value == ADC_VREF == 3.3 V
	adc_select_input(1);
	const float conversion_factor = 3.3f / (1 << 12);
	uint16_t result = adc_read();
	return (result * conversion_factor);
}

void __not_in_flash_func(adc_capture)(uint16_t *buf, size_t count)
{
	adc_fifo_setup(true, false, 0, false, false);
	adc_run(true);
	for (int i = 0; i < count; i = i + 1)
		buf[i] = adc_fifo_get_blocking();
	adc_run(false);
	adc_fifo_drain();
}

#define NSAMP 1000
void adc_print_audio_record(void)
{
	uint16_t adc_buf[NSAMP];

	adc_run(false);
	adc_select_input(0);
	adc_capture(adc_buf, NSAMP);
	printf("audio:\r\n");
	for (int ii = 0; ii < NSAMP; ii++)
	{
		printf("%d,", adc_buf[ii]);
	}
	printf("\r\n");
	adc_run(false);
}
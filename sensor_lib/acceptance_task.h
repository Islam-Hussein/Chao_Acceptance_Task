#ifndef ACCEPTANCE_TASK_H_
#define ACCEPTANCE_TASK_H_


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/i2c.h"

#include "aceeptance_cfg.h"

void ADC_Set_SamplingRate(void);

void ADC_Input_Init(void);

void ADC_Sample_Capture(void);



void ADXLl345_Init(void);

void adxl345_ReadData(int16_t *xAccl, int16_t *yAccl, int16_t *zAccl);




#endif
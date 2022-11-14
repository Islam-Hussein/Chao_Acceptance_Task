//
// Created by chao on 12/09/2021.
//

#ifndef MY_PROJECT_SHT31_H
#define MY_PROJECT_SHT31_H
#include "hardware/i2c.h"

int sht31_read_temp_hum(float *temp, float *humidity);
int sht31_init(i2c_inst_t *i2c);
#endif //MY_PROJECT_SHT31_H

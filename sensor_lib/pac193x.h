//
// Created by chao on 13/09/2021.
//

#ifndef TESTSENSORBOARD_PAC193X_H
#define TESTSENSORBOARD_PAC193X_H
#include <stdint.h>
#include "hardware/i2c.h"
#define PAC193X_SLAVE_ADDR 0x11 // when the addr_resistor is 499 Ohm

typedef struct PAC193X_Info {
    uint8_t product_id;
    uint8_t manufacturer_id;
    uint8_t revision_id;
} PAC193X_Info;

void pac193x_print_info(void);
PAC193X_Info pac193x_get_info(void);
int pac193x_init(i2c_inst_t *i2c);

#endif //TESTSENSORBOARD_PAC193X_H

//
// Created by chao on 12/09/2021.
//

#include "sht31.h"

#include <stdio.h>


#define SHT31_SLAVE_ADDR 0x44 /**< SHT31 Default Address */

#define SHT31_MEAS_HIGHREP_STRETCH                                             \
  0x2C06 /**< Measurement High Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_MEDREP_STRETCH                                              \
  0x2C0D /**< Measurement Medium Repeatability with Clock Stretch Enabled */
#define SHT31_MEAS_LOWREP_STRETCH                                              \
  0x2C10 /**< Measurement Low Repeatability with Clock Stretch Enabled*/
#define SHT31_MEAS_HIGHREP                                                     \
  0x2400 /**< Measurement High Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_MEDREP                                                      \
  0x240B /**< Measurement Medium Repeatability with Clock Stretch Disabled */
#define SHT31_MEAS_LOWREP                                                      \
  0x2416 /**< Measurement Low Repeatability with Clock Stretch Disabled */
#define SHT31_READSTATUS 0xF32D   /**< Read Out of Status Register */
#define SHT31_CLEARSTATUS 0x3041  /**< Clear Status */
#define SHT31_SOFTRESET 0x30A2    /**< Soft Reset */
#define SHT31_HEATEREN 0x306D     /**< Heater Enable */
#define SHT31_HEATERDIS 0x3066    /**< Heater Disable */
#define SHT31_REG_HEATER_BIT 0x0d /**< Status Register Heater Bit */

static i2c_inst_t *I2C_CH_SHT31;


/*! \brief  Check if the sht31 is on the I2C Bus
 *
 * \param none
 *
 * \returns -1: sensor is offline
 *          else: sensor on line
 */
int sht31_on_bus(uint8_t device_slave_addr)
{
    int ret;
    uint8_t cmdbuffer[2]={0x2C, 0x06};
    ret = i2c_write_blocking(I2C_CH_SHT31, SHT31_SLAVE_ADDR, cmdbuffer, 2, false);
    return ret;
}

/*! \brief Initilize the sht31 through the I2C bus
 *
 * \param i2c: could only be i2c0 or i2c1
 *
 * \returns -1: sensor is offline
 *          else: sensor is initialized
 */
int sht31_init(i2c_inst_t *i2c) {
    uint8_t cmdbuffer[2];
    I2C_CH_SHT31 = i2c;

    if (sht31_on_bus(SHT31_SLAVE_ADDR) < 0) {
        return -1;
    }
    sleep_ms(2);
    return 1;
}

/**
 * Performs a CRC8 calculation on the supplied values.
 *
 * @param data  Pointer to the data to use when calculating the CRC8.
 * @param len   The number of bytes in 'data'.
 *
 * @return The computed CRC8 value.
 */
static uint8_t crc8(const uint8_t *data, int len) {
    /*
     * CRC-8 formula from page 14 of SHT spec pdf
     * Test data 0xBE, 0xEF should yield 0x92
     *
     * Initialization data 0xff
     * Polynomial 0x31 (x8 + x5 + x4 +1)
     * Final XOR 0x00
     */


    uint8_t crc = 0xff;

    for (int j = len; j; --j) {
        crc ^= *data++;

        for (int i = 8; i; --i) {
            crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);
        }
    }
    return crc;
}



/**
 * Perform a temp + humidity read.
 *
 * @return True if successful, otherwise false.
 */
int sht31_read_temp_hum(float *temp, float *humidity) {
    int ret;
    uint8_t readbuffer[6];
    uint8_t cmdbuffer[2]={0x2C, 0x06};

    i2c_write_blocking(I2C_CH_SHT31, SHT31_SLAVE_ADDR, cmdbuffer, 2, false);

    sleep_ms(20);

    ret = i2c_read_blocking(I2C_CH_SHT31, SHT31_SLAVE_ADDR, readbuffer, sizeof(readbuffer), false);

    if (readbuffer[2] != crc8(readbuffer, 2) ||
        readbuffer[5] != crc8(readbuffer + 3, 2)){
        ret=-1;
        return ret;
    }

    int32_t stemp = (int32_t)(((uint32_t)readbuffer[0] << 8) | readbuffer[1]);
    // simplified (65536 instead of 65535) integer version of:
    // temp = (stemp * 175.0f) / 65535.0f - 45.0f;
    stemp = ((4375 * stemp) >> 14) - 4500;
    *temp = (float)stemp / 100.0f;

    uint32_t shum = ((uint32_t)readbuffer[3] << 8) | readbuffer[4];
    // simplified (65536 instead of 65535) integer version of:
    // humidity = (shum * 100.0f) / 65535.0f;
    shum = (625 * shum) >> 12;
    *humidity = (float)shum / 100.0f;

    return true;
}



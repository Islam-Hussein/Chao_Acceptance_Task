//
// Created by chao on 13/09/2021.
//

#include "pac193x.h"
#include <stdio.h>
#include "pico/stdlib.h"

// PAC193x register addresses
#define PAC1934_REFRESH_CMD_ADDR            0x00
#define PAC1934_CTRL_ADDR                   0x01
#define PAC1934_ACC_COUNT_ADDR              0x02
#define PAC1934_VPOWER1_ACC_ADDR            0x03
#define PAC1934_VPOWER2_ACC_ADDR            0x04
#define PAC1934_VPOWER3_ACC_ADDR            0x05
#define PAC1934_VPOWER4_ACC_ADDR            0x06
#define PAC1934_VBUS1_ADDR                  0x07
#define PAC1934_VBUS2_ADDR                  0x08
#define PAC1934_VBUS3_ADDR                  0x09
#define PAC1934_VBUS4_ADDR                  0x0A
#define PAC1934_VSENSE1_ADDR                0x0B
#define PAC1934_VSENSE2_ADDR                0x0C
#define PAC1934_VSENSE3_ADDR                0x0D
#define PAC1934_VSENSE4_ADDR                0x0E
#define PAC1934_VBUS1_AVG_ADDR              0x0F
#define PAC1934_VBUS2_AVG_ADDR              0x10
#define PAC1934_VBUS3_AVG_ADDR              0X11
#define PAC1934_VBUS4_AVG_ADDR              0X12
#define PAC1934_VSENSE1_AVG_ADDR            0X13
#define PAC1934_VSENSE2_AVG_ADDR            0X14
#define PAC1934_VSENSE3_AVG_ADDR            0X15
#define PAC1934_VSENSE4_AVG_ADDR            0X16
#define PAC1934_VPOWER1_ADDR                0X17
#define PAC1934_VPOWER2_ADDR                0X18
#define PAC1934_VPOWER3_ADDR                0X19
#define PAC1934_VPOWER4_ADDR                0X1A
#define PAC1934_CHANNEL_DIS_ADDR            0X1C
#define PAC1934_NEG_PWR_ADDR                0X1D
#define PAC1934_REFRESH_G_CMD_ADDR          0x1E
#define PAC1934_REFRESH_V_CMD_ADDR          0x1F
#define PAC1934_SLOW_ADDR                   0X20
#define PAC1934_CTRL_ACT_ADDR               0X21
#define PAC1934_CHANNEL_DIS_ACT_ADDR        0X22
#define PAC1934_NEG_PWR_ACT_ADDR            0X23
#define PAC1934_CTRL_LAT_ADDR               0X24
#define PAC1934_CHANNEL_DIS_LAT_ADDR        0X25
#define PAC1934_NEG_PWR_LAT_ADDR            0x26

#define PAC1934_PRODUCT_ID_ADDR             0xFD
#define PAC1934_MANUFACTURER_ID_ADDR        0xFE
#define PAC1934_REVISION_ID_ADDR            0xFF

// I2C instance
static i2c_inst_t *I2C_CH;

/*! \brief  Power on/off the sensor if it is necessary
 *
 * \param none
 *
 * \returns 1: power on the sensor
 *          0: sensor off the sensor
 */
int pac193x_power_ctrl(uint8_t power_enable)
{
    gpio_init(29);
    gpio_set_dir(29, GPIO_OUT);
    gpio_put(29, power_enable);
}


/*! \brief  Check if the PAC193x is on the I2C Bus
 *
 * \param none
 *
 * \returns -1: sensor is offline
 *          else: sensor on line
 */
int pac193x_on_bus(uint8_t device_slave_addr)
{
    int ret;
    uint8_t readbuffer[2];
    ret = i2c_read_blocking(I2C_CH, device_slave_addr, readbuffer, 1, false);
    return ret;
}

/*! \brief apply REFRESH command to pac193x
 *         the accumulator data, accumulator count, Vbus,
 *         Vsense measurements are all refreshed
 *         and the accumulators are reset.
 *
 * \param none
 *
 * \returns none
 */
void pac193x_refresh(void) {
    uint8_t cmdbuffer[2];
    cmdbuffer[0] = PAC1934_REFRESH_CMD_ADDR;
    cmdbuffer[1] = 0x01;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 2, false);
}

/*! \brief Initilize the PAC193x through the I2C bus
 *
 * \param i2c: could only be i2c0 or i2c1
 *
 * \returns -1: sensor is offline
 *          else: sensor is initialized
 */
int pac193x_init(i2c_inst_t *i2c)
{
    uint8_t cmdbuffer[2];
    I2C_CH = i2c;

    pac193x_power_ctrl(1);
    sleep_ms(10);

    if(pac193x_on_bus(PAC193X_SLAVE_ADDR)<0){
        return -1;
    }


    cmdbuffer[0] = PAC1934_NEG_PWR_ADDR;
    cmdbuffer[1] = 0x00;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 2, false);

    cmdbuffer[0] = PAC1934_CTRL_ADDR;
    cmdbuffer[1] = 0x02;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 2, false);

    cmdbuffer[0] = PAC1934_CHANNEL_DIS_ADDR;
    cmdbuffer[1] = 0x00;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 2, false);

    // configure the function of the SLOW PIN
    cmdbuffer[0] = PAC1934_SLOW_ADDR;
    cmdbuffer[1] = 0x14;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 2, false);

    pac193x_refresh();
    sleep_ms(1);
    return 1;
}


/*! \brief Get the device information
 *
 * \param none
 *
 * \outputs product_id: pass the product id of pac193x
 *          manufacturer_id: pass the manufacturer id of pac193x
 *          revision_id: pass the revision id of pac193x
 *
 * \returns none
 */
PAC193X_Info pac193x_get_info(void)
{
    uint8_t cmdbuffer[2];
    PAC193X_Info info;

    //0x58 for PAC1931
    //0x59 for PAC1932
    //0x5a for PAC1933
    //0x5b for PAC1934
    cmdbuffer[0] = PAC1934_PRODUCT_ID_ADDR;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 1, false);
    i2c_read_blocking(I2C_CH, PAC193X_SLAVE_ADDR, &info.product_id, 1, false);

    // should be always 0x5d
    cmdbuffer[0] = PAC1934_MANUFACTURER_ID_ADDR;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 1, false);
    i2c_read_blocking(I2C_CH, PAC193X_SLAVE_ADDR, &info.manufacturer_id, 1, false);

    // reversion id
    cmdbuffer[0] = PAC1934_REVISION_ID_ADDR;
    i2c_write_blocking(I2C_CH, PAC193X_SLAVE_ADDR, cmdbuffer, 1, false);
    i2c_read_blocking(I2C_CH, PAC193X_SLAVE_ADDR, &info.revision_id, 1, false);
    return info;
}





/*! \brief Print the device information
 *
 * \param none
 *
 * \outputs through the printf() in stdio.
 *
 * \returns none
 */
void pac193x_print_info(void)
{
    PAC193X_Info info;

    info = pac193x_get_info();

    printf("Product id: 0x%2x\r\nManufacture id: 0x%2x\r\nRevision id: 0x%02x\r\n", info.product_id, info.manufacturer_id, info.revision_id);
}
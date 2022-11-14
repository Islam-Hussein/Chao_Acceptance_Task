#include "acceptance_task.h"

extern uint8_t capture_buf[];

extern uint8_t *sample_address_pointer;

static i2c_inst_t *I2C_CH_ADXL345;

/*=========================================================================
    I2C ADDRESS/BITS
    -----------------------------------------------------------------------*/
#define ADXL345_DEFAULT_ADDRESS (0x53) ///< Assumes ALT address pin low
/*=========================================================================*/

/*=========================================================================
    REGISTERS
    -----------------------------------------------------------------------*/
#define ADXL345_REG_DEVID (0x00) ///< Device ID
#define ADXL345_REG_THRESH_TAP (0x1D) ///< Tap threshold
#define ADXL345_REG_OFSX (0x1E) ///< X-axis offset
#define ADXL345_REG_OFSY (0x1F) ///< Y-axis offset
#define ADXL345_REG_OFSZ (0x20) ///< Z-axis offset
#define ADXL345_REG_DUR (0x21) ///< Tap duration
#define ADXL345_REG_LATENT (0x22) ///< Tap latency
#define ADXL345_REG_WINDOW (0x23) ///< Tap window
#define ADXL345_REG_THRESH_ACT (0x24) ///< Activity threshold
#define ADXL345_REG_THRESH_INACT (0x25) ///< Inactivity threshold
#define ADXL345_REG_TIME_INACT (0x26) ///< Inactivity time
#define ADXL345_REG_ACT_INACT_CTL (0x27) ///< Axis enable control for activity and inactivity detection
#define ADXL345_REG_THRESH_FF (0x28) ///< Free-fall threshold
#define ADXL345_REG_TIME_FF (0x29) ///< Free-fall time
#define ADXL345_REG_TAP_AXES (0x2A) ///< Axis control for single/double tap
#define ADXL345_REG_ACT_TAP_STATUS (0x2B) ///< Source for single/double tap
#define ADXL345_REG_BW_RATE (0x2C) ///< Data rate and power mode control
#define ADXL345_REG_POWER_CTL (0x2D) ///< Power-saving features control
#define ADXL345_REG_INT_ENABLE (0x2E) ///< Interrupt enable control
#define ADXL345_REG_INT_MAP (0x2F) ///< Interrupt mapping control
#define ADXL345_REG_INT_SOURCE (0x30) ///< Source of interrupts
#define ADXL345_REG_DATA_FORMAT (0x31) ///< Data format control
#define ADXL345_REG_DATAX0 (0x32) ///< X-axis data 0
#define ADXL345_REG_DATAX1 (0x33) ///< X-axis data 1
#define ADXL345_REG_DATAY0 (0x34) ///< Y-axis data 0
#define ADXL345_REG_DATAY1 (0x35) ///< Y-axis data 1
#define ADXL345_REG_DATAZ0 (0x36) ///< Z-axis data 0
#define ADXL345_REG_DATAZ1 (0x37) ///< Z-axis data 1
#define ADXL345_REG_FIFO_CTL (0x38) ///< FIFO control


void ADC_Set_SamplingRate(void)
{adc_set_clkdiv(48000000 / SAMPLING_RATE);}

void ADC_Input_Init(void)
{
    adc_init();

    // Init GPIO for analogue use: hi-Z, no pulls, disable digital input buffer.
    adc_gpio_init(26 + CAPTURE_CHANNEL);

    adc_select_input(CAPTURE_CHANNEL);

    adc_fifo_setup(
        true,  // Write each completed conversion to the sample FIFO
        true,  // Enable DMA data request (DREQ)
        1,     // DREQ (and IRQ) asserted when at least 1 sample present
        false, // We won't see the ERR bit because of 8 bit reads; disable.
        true   // Shift each sample to 8 bits when pushing to FIFO
    );

    }

void ADC_Sample_Capture(void)
{
    
    dma_channel_config cfg = dma_channel_get_default_config(DMA_CHANNEL);


    // Reading from constant address, writing to incrementing byte addresses
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_8);
    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    // Pace transfers based on availability of ADC samples
    channel_config_set_dreq(&cfg, DREQ_ADC);

    dma_channel_configure(
        DMA_CHANNEL,
        &cfg,
        //sample_address_pointer,   // dst
        capture_buf,
        &adc_hw->fifo,            // src
        SAMPLING_BUFFER_SIZE,     // transfer count
        true                      // start immediately
    );
    

    adc_run(true);

    dma_channel_wait_for_finish_blocking(DMA_CHANNEL);

    
    for(int i = 0 ; i < SAMPLING_BUFFER_SIZE ; i += 10)
    {
        printf("%d \n" , capture_buf[i]);
    }
    
    adc_run(false);
    adc_fifo_drain();

}


void ADXLl345_Init(void)
{
    uint8_t cmd_buffer[2];

    I2C_CH_ADXL345 = i2c0;


    // Select Bandwidth ate register(0x2C)
    // Normal mode, Output data rate = 100 Hz(0x0A)
    cmd_buffer[0]=  0x2C;
    cmd_buffer[1] = 0x0A;
    i2c_write_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, cmd_buffer, 2, false);

    // Adjust FIFO Mode to Triggered Mode as we don't have INT1 or INT2 connected to pico
    // Adjust Max. no. OF Samples to 32 Sample
    cmd_buffer[0]=  0x38;
    cmd_buffer[1] = 0xDF;
    i2c_write_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, cmd_buffer, 2, false);


    // Select Power control register(0x2D)
    // Auto-sleep disable(0x08)
    cmd_buffer[0] = 0x2D;
    cmd_buffer[1] = 0x08;
    i2c_write_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, cmd_buffer, 2, false);

    // Select Data format register(0x31)
    // Self test disabled, 4-wire interface, Full resolution, range = +/-2g(0x08)
    cmd_buffer[0] = 0x31;
    cmd_buffer[1] = 0x08;
    i2c_write_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, cmd_buffer, 2, false);

    sleep_ms(10);

}

void adxl345_ReadData(int16_t *xAccl, int16_t *yAccl, int16_t *zAccl)
{
    uint8_t readbuffer[6];
    uint8_t cmdbuffer[2];
    int16_t acc_x, acc_y, acc_z;

    // Read 6 bytes of data from register(0x32)
    // xAccl lsb, xAccl msb, yAccl lsb, yAccl msb, zAccl lsb, zAccl msb
    cmdbuffer[0] = 0x32;
    i2c_write_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, cmdbuffer, 1, false);
    i2c_read_blocking(I2C_CH_ADXL345, ADXL345_SLAVE_ADDR, readbuffer, sizeof(readbuffer), false);

    // Convert the data to 10-bits
    acc_x = (int16_t)((readbuffer[1] & 0x03) * 256 + (readbuffer[0] & 0xFF));
    if(acc_x > 511)
        acc_x = (int16_t)(acc_x-1024);

    acc_y = (int16_t)((readbuffer[3] & 0x03) * 256 + (readbuffer[2] & 0xFF));
    if(acc_y > 511)
    {
        acc_y = (int16_t)(acc_y-1024);
    }

    acc_z = (int16_t)((readbuffer[5] & 0x03) * 256 + (readbuffer[4] & 0xFF));
    if(acc_z > 511)
    {
        acc_z = (int16_t)(acc_z-1024);
    }

    *xAccl = acc_x;
    *yAccl = acc_y;
    *zAccl = acc_z;
}
        
    








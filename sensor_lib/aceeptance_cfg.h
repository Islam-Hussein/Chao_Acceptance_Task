#ifndef ACCEPTANCE_CFG_H_
#define ACCEPTANCE_CFG_H_


#include "hardware/i2c.h"

/*          ADC   Configurations        */

#define SAMPLING_RATE           60000   //Controls ADC_CLK (How many samples to take per Second) 
#define TIME                    1000    //1 second (1000ms)
//#define SAMPLING_BUFFER_SIZE    (uint32_t)(SAMPLING_RATE * TIME/ 1000)     //Controls Size of captured Buffer
#define SAMPLING_BUFFER_SIZE    2048     //Controls Size of captured Buffer
#define DMA_CHANNEL             1       //Determine which DMA Channel to Be Used
#define CAPTURE_CHANNEL         1       //Determine which ADC_Pin to be used    1 --> Potontiometer  0 --> MIC



/*          ADXL345   Configurations        */

#define ADXL345_SLAVE_ADDR 0x53 // when ALT is connected to GND
#define I2C_FOR_ADXL345 i2c0





#endif
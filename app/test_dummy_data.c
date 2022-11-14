#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#define LEN 512
static uint8_t period[4] = {16, 32, 64, 128};
// When this code is running on Pico MCU, and the MCU
// is connected through screen(or Putty), per second
// the MCU will send "hello" to the screen.
int main(){
	// Enable UART so we can print status output
	stdio_init_all();
	uint16_t buf[LEN];

	uint8_t cnt = 0;
	while(1){

		for(int i=0;i<LEN;i++){
			buf[i] = 127*sin(i*3.14/period[cnt])+128;
		}

		for(int i=0;i<LEN;i++){
			printf("%d,", buf[i]);
		}
		printf("\n");
		sleep_ms(1000);

		cnt++;
		if(cnt==4){
			cnt=0;
		}
	}
}

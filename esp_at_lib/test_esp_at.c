#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "pico/bootrom.h"
#include "esp_at.h"
#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define PRINT(fmt,...) {printf("[%s:%d <%s>]",__FILENAME__,__LINE__, __FUNCTION__); printf(fmt, ##__VA_ARGS__);printf("\n");}


// So if you send 5 'r', the MCU will enter bootloader mode.
// or you could use command as, echo "rrrrrr" > /dev/ttyACM0
void enter_boot_mode(char input_char){
    static char reset_cnt = 0;
    if(input_char=='r'){
        reset_cnt++;
        if(reset_cnt>=5){
            reset_usb_boot(0, 0);
        }
    }else{
        reset_cnt = 0;
    }
}

int main() {
    bool enable_esp_process = false;

    // Enable UART so we can print status output
    stdio_init_all();

    while (1) {
        char input_char = getchar_timeout_us(10000);
        // ASCII code from 32(Dec) to 126 will be echo back
        if (input_char >= 32 && input_char <= 126) {

            enter_boot_mode(input_char);
            if(input_char=='t'){

                PRINT("Start test ESP AT========");
                enable_esp_process = true;

            }else{
                PRINT("keyboard input: '%c' or in decimal %d", input_char, input_char);
            }

            // sleep_ms(1000);
        } else {
            // Uncomment the line below for testing
            // printf("i dont know what you typed:%d.\n", input_char);
        }

        if(enable_esp_process)
            esp_at_process();
    }
}

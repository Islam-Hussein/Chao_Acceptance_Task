//
// Created by chao on 09/11/2021.
//
#include "uart_to_esp.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "string.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define PRINT(fmt,...) {printf("[%s:%d <%s>]",__FILENAME__,__LINE__, __FUNCTION__); printf(fmt, ##__VA_ARGS__);printf("\n");}

//========UART parameter===========================
typedef struct UART_Dev {
    char name[15];
    uart_inst_t *uart_id;
    uint uart_irq;
    uint tx_pin;
    uint rx_pin;
    uint baud_rate_set;
    uint baud_rate_actual;
    uint data_bits;
    uint stop_bits;
    uart_parity_t parity;
    char receive_buf[1000];
    int receive_count;
} UART_Dev_t;

static UART_Dev_t esp32_uart_dev = {
        // Below depends on the hardware connection
        // you should modify it according to your hardware
        .name = "uart_to_esp32",
        .uart_id = uart1,
        .uart_irq = UART1_IRQ,
        .tx_pin = 4,
        .rx_pin = 5,

        // Below depends on the firmware on the esp32 module
        .baud_rate_set = 115200,
        .baud_rate_actual = 0,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = UART_PARITY_NONE,
        .receive_count=0,
};
//========UART parameter===========================



// RX interrupt handler
void callback_uart_rx_interrupt() {
    while (uart_is_readable(esp32_uart_dev.uart_id)) {
        uint8_t ch = uart_getc(esp32_uart_dev.uart_id);
        esp32_uart_dev.receive_buf[esp32_uart_dev.receive_count] = ch;
        esp32_uart_dev.receive_count++;
        esp32_uart_dev.receive_buf[esp32_uart_dev.receive_count] = 0;
//        if(ch=='\n'){
//            esp32_uart_dev.receive_buf[esp32_uart_dev.receive_count] = 0;
//            PRINT("%s", esp32_uart_dev.receive_buf);
////            // raw data print
////            for(int i=0;i<recv_pointer;i++){
////                printf("%02x ", recv_buf[i]);
////            }
//        }
    }
}


void uart_to_esp_init(void){

//    PRINT("Start initialize the uart interface which connect to esp32.");

    // Set up our UART with a basic baud rate.
    uart_init(esp32_uart_dev.uart_id, esp32_uart_dev.baud_rate_set);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(esp32_uart_dev.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(esp32_uart_dev.rx_pin, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    esp32_uart_dev.baud_rate_actual = uart_set_baudrate(esp32_uart_dev.uart_id, esp32_uart_dev.baud_rate_set);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(esp32_uart_dev.uart_id, false, false);

    // Set our data format
    uart_set_format(esp32_uart_dev.uart_id, esp32_uart_dev.data_bits, esp32_uart_dev.stop_bits, esp32_uart_dev.parity);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(esp32_uart_dev.uart_id, false);

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(esp32_uart_dev.uart_irq, callback_uart_rx_interrupt);
    irq_set_enabled(esp32_uart_dev.uart_irq, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(esp32_uart_dev.uart_id, true, false);

//    PRINT("done.");
}


void uart_to_esp_send_data(uint8_t *data, uint32_t len){
    uart_write_blocking(esp32_uart_dev.uart_id, data, len);
}

void uart_to_esp_send_string(char *data){
    uart_puts(esp32_uart_dev.uart_id, data);
}

void uart_to_esp_send_string_line(char *data){
    uart_puts(esp32_uart_dev.uart_id, data);
    uart_puts(esp32_uart_dev.uart_id, "\r\n");
}

void uart_to_esp_clean_receive_buffer(void){
//    memset(esp32_uart_dev.receive_buf, 0, esp32_uart_dev.receive_count);
    esp32_uart_dev.receive_buf[0]=0; // more efficient
    esp32_uart_dev.receive_count = 0;
}

bool uart_to_esp_response_arrived(char *expected_response){
    return (strstr(esp32_uart_dev.receive_buf,expected_response)!=NULL);
}

// ideally: "+PING:200\r\n"
// the fact: "\r\n\r\n+PING:200\r\n"
//           or  "\r\n \r\n+PING:200\r\n"
//           or  "\r\n  \r\n +PING:200\r\n"
// we have to search the position of we can search the position of "+PING"

bool uart_to_esp_read_line(char *line_start_str, char *data){
//    PRINT("try to read");
    if (esp32_uart_dev.receive_count>0){
//        PRINT(esp32_uart_dev.receive_buf);
        char* p_line_start = strstr(esp32_uart_dev.receive_buf, line_start_str);
        if(p_line_start!=NULL){
            char* p_line_end = strstr(p_line_start, "\r\n");
            if(p_line_end!=NULL){
                *p_line_end = '\0';
                memcpy(data, p_line_start+strlen(line_start_str), p_line_end-p_line_start+1);
                return true;
            }
        }
    }

    return false;
}
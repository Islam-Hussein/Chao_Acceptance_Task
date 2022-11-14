#include "pico/stdlib.h"
#include "stdio.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "pico/bootrom.h"

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define PRINT(fmt,...) {printf("[%s:%d <%s>]",__FILENAME__,__LINE__, __FUNCTION__); printf(fmt, ##__VA_ARGS__);printf("\n");}

#define UART_ID uart1
#define BAUD_RATE 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY    UART_PARITY_NONE

// We are using pins 4 and 5, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

static int chars_rxed = 0;
char recv_buf[100]= {};
int recv_pointer=0;
// RX interrupt handler
void on_uart_rx() {
    while (uart_is_readable(UART_ID)) {
        uint8_t ch = uart_getc(UART_ID);
        recv_buf[recv_pointer] = ch;
        recv_pointer++;
        if(ch=='\n'){
            recv_buf[recv_pointer] = 0;
//            PRINT("%s",recv_buf);
            for(int i=0;i<recv_pointer;i++){
                printf("%02x ", recv_buf[i]);
            }
            recv_pointer=0;
        }
    }
}

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
    // Enable UART so we can print status output
    stdio_init_all();

    recv_pointer = 0;

    // Set up our UART with a basic baud rate.
    uart_init(UART_ID, 2400);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Actually, we want a different speed
    // The call will return the actual baud rate selected, which will be as close as
    // possible to that requested
    int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

    // Set UART flow control CTS/RTS, we don't want these, so turn them off
    uart_set_hw_flow(UART_ID, false, false);

    // Set our data format
    uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

    // Turn off FIFO's - we want to do this character by character
    uart_set_fifo_enabled(UART_ID, false);

    // Set up a RX interrupt
    // We need to set up the handler first
    // Select correct interrupt for the UART we are using
    int UART_IRQ = UART_ID == uart0 ? UART0_IRQ : UART1_IRQ;

    // And set up and enable the interrupt handlers
    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(UART_ID, true, false);

    // OK, all set up.
    // Lets send a basic string out, and then run a loop and wait for RX interrupts
    // The handler will count them, but also reflect the incoming data back with a slight change!
//    uart_puts(UART_ID, "\nHello, uart interrupts\n");

    while (1) {
        char input_char = getchar_timeout_us(10000);
        // ASCII code from 32(Dec) to 126 will be echo back
        if (input_char >= 32 && input_char <= 126) {

            enter_boot_mode(input_char);
            if(input_char=='t'){
                PRINT("send AT");
                uart_puts(UART_ID, "AT\r\n");
            }else{
                PRINT("keyboard input: '%c' or in decimal %d", input_char, input_char);
            }

            // sleep_ms(1000);
        } else {
            // Uncomment the line below for testing
            // printf("i dont know what you typed:%d.\n", input_char);
        }


    }
}

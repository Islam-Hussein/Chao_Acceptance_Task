//
// Created by chao on 09/11/2021.
//

#ifndef MY_PROJECT_UART_TO_ESP_H
#define MY_PROJECT_UART_TO_ESP_H
#include <stdint.h>
#include <stdbool.h>

void uart_to_esp_init(void);

void uart_to_esp_send_data(uint8_t *data, uint32_t len);
void uart_to_esp_send_string(char *data);
void uart_to_esp_send_string_line(char *data);
void uart_to_esp_send_string(char *data);

void uart_to_esp_clean_receive_buffer(void);
bool uart_to_esp_response_arrived(char *expected_response);
bool uart_to_esp_read_line(char *line_start_str, char *data);
#endif //MY_PROJECT_UART_TO_ESP_H

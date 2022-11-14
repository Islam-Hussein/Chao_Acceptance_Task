//
// Created by chao on 09/11/2021.
//

#include "esp_at.h"
#include "uart_to_esp.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include <string.h>

#define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)
#define PRINT(fmt,...) {printf("[%s:%d <%s>]",__FILENAME__,__LINE__, __FUNCTION__); printf(fmt, ##__VA_ARGS__);printf("\n");}


//========ESP-AT parameter===========================
typedef struct ESP_AT_Dev {
    char name[15];
    char ssid[30];
    char password[30];
} ESP_AT_Dev_t;


static ESP_AT_Dev_t esp_at_dev =  {
        // you should change it,
        // before you push code maybe change this back to a dummy password
        .name = "asp_at",
        .ssid =  "your wifi ssid",
        .password = "your password"

};
//========ESP-AT parameter===========================

typedef enum {ESP_STATE_POWER_RESET=0,
              ESP_STATE_ACK_CHECK,
              ESP_STATE_SOFT_RESET,
              ESP_STATE_SET_STATION_MODE,
              ESP_STATE_CONNECT_TO_AP,
              ESP_STATE_IDLE
}esp_state_t;
static uint8_t esp_state = ESP_STATE_POWER_RESET;

// Todo: add the retry times in the interface
bool send_at_command(char *cmd, char *expected_response, uint32_t timeout_ms, uint32_t max_retry)
{
    uint8_t retry_count = max_retry;
    uint32_t delay;
    bool response_arrived = false;

    // Clean the uart receive buffer before send the command
    uart_to_esp_clean_receive_buffer();

    while((retry_count--)&&(!response_arrived)){
        // send the command
        uart_to_esp_send_string_line(cmd);
        for(delay=0; delay<timeout_ms; delay++)
        {
            response_arrived = uart_to_esp_response_arrived(expected_response);

            sleep_ms(1);

//            if(response_arrived)
//                break;
        }
    }

    return response_arrived;
}

void esp_at_power_reset(void){
    sleep_ms(1000);
}

void esp_at_soft_reset(void){
    send_at_command("AT+RST", "OK", 100, 5);
    sleep_ms(2000);  // wait until it is ready
}

bool esp_at_connect_to_ap(char *ssid, char *pwd){
    char cmd_buffer[100];

    // cmd =  AT+CWJAP="ES-Air_2G4","xxxxxxxxxxxxxxx"\r\n
    strcpy(cmd_buffer, "AT+CWJAP=\"");
    strcat(cmd_buffer, ssid);
    strcat(cmd_buffer, "\",\"");
    strcat(cmd_buffer, pwd);
    strcat(cmd_buffer, "\"");

//    PRINT(cmd_buffer);
    return(send_at_command(cmd_buffer, "WIFI GOT IP", 2000, 1));
}


void esp_at_ping(char *host_ip){
    char cmd_buffer[30];

    strcpy(cmd_buffer, "AT+PING=\"");
    strcat(cmd_buffer, host_ip);
    strcat(cmd_buffer, "\"");

    if(send_at_command(cmd_buffer, "+PING:", 1000, 2)){
        char time_cost_string[20];
        uart_to_esp_read_line("+PING:", time_cost_string);
        PRINT("PING host IP %s, %sms",host_ip, time_cost_string);
    }else{
        PRINT("ERROR: ESP32 is not responding.");
    }

}

// Todo: A stat machine about esp32 connecting to cloud.
void esp_at_process(void){
    switch (esp_state) {
        case ESP_STATE_POWER_RESET:
            PRINT("ESP_STATE_POWER_RESET");
            uart_to_esp_init();   // init the uart interface for at
            esp_at_power_reset(); // do hardware reset
            esp_state = ESP_STATE_SOFT_RESET;
            break;
        case ESP_STATE_SOFT_RESET:
            PRINT("ESP_STATE_SOFT_RESET");
            esp_at_soft_reset();
            // disable echo, make the output more clean
            send_at_command("ATE0", "OK", 100, 5);
            esp_state = ESP_STATE_ACK_CHECK;
            break;
        case ESP_STATE_ACK_CHECK:
            if(send_at_command("AT", "OK", 100, 5)){
                PRINT("ESP_STATE_ACK_CHECK: SUCCESS");

                esp_state = ESP_STATE_SET_STATION_MODE;
            }else{
                PRINT("ESP_STATE_ACK_CHECK: FAIL");
                sleep_ms(1000);
            }
            break;
        case ESP_STATE_SET_STATION_MODE:
            if(send_at_command("AT+CWMODE=1", "OK", 100, 5)){
                PRINT("ESP_STATE_SET_STATION_MODE: SUCCESS");
                esp_state = ESP_STATE_CONNECT_TO_AP;
            }else{
                PRINT("ESP_STATE_SET_STATION_MODE: FAIL");
                sleep_ms(1000);
            }
            break;
        case ESP_STATE_CONNECT_TO_AP:
            if(esp_at_connect_to_ap(esp_at_dev.ssid, esp_at_dev.password)){
                PRINT("ESP_STATE_CONNECT_TO_AP: SUCCESS");
                esp_state = ESP_STATE_IDLE;
            }else{
                PRINT("ESP_STATE_CONNECT_TO_AP: FAIL");
                sleep_ms(1000);
            }
            break;
        case ESP_STATE_IDLE:

            esp_at_ping("192.168.0.222");
            sleep_ms(1000);
            break;
        default:
            // do nothing
            break;
    }
}

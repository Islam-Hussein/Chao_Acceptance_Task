## You must modify the wifi SSID and Password accordingly
**DO NOT commit a real password to gitlab.**
```c
static ESP_AT_Dev_t esp_at_dev =  {
// you should change it,
// before you push code maybe change this back to a dummy password
.name = "asp_at",
.ssid =  "chao_home",
.password = "dummy_password"

};
```

## How to enter the bootloader to upload code
Please have a look the main function in `test_esp_at.c`, if you press the key 'r' on you keyboard for 5 times, the MCU 
can switch to bootloader mode. Therefore you don't need to press the boot key anymore.

**Note:** if you have a `sleep_ms()` function in your while loop, the receiving the 'r' character would take more time. 
For example once the module connected to wifi, and do `ping`, and you press the 'r' for 5 time, it would take about 10 
seconds then the module will receive all 'r'.


## Some test out put
```shell
[test_esp_at.c:39 <main>]Start test ESP AT========
[esp_at.c:114 <esp_at_process>]ESP_STATE_POWER_RESET
[esp_at.c:120 <esp_at_process>]ESP_STATE_SOFT_RESET
[esp_at.c:128 <esp_at_process>]ESP_STATE_ACK_CHECK: SUCCESS
[esp_at.c:138 <esp_at_process>]ESP_STATE_SET_STATION_MODE: SUCCESS
[esp_at.c:150 <esp_at_process>]ESP_STATE_CONNECT_TO_AP: FAIL
[esp_at.c:147 <esp_at_process>]ESP_STATE_CONNECT_TO_AP: SUCCESS
[esp_at.c:103 <esp_at_ping>]PING host IP 192.168.0.222, 423ms
[esp_at.c:103 <esp_at_ping>]PING host IP 192.168.0.222, 341ms
[esp_at.c:103 <esp_at_ping>]PING host IP 192.168.0.222, 8ms
[esp_at.c:103 <esp_at_ping>]PING host IP 192.168.0.222, 34ms
[esp_at.c:103 <esp_at_ping>]PING host IP 192.168.0.222, 285ms
```
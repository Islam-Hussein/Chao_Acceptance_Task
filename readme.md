# Before you use anycode in this branch
1. This branch should not be used unless you are testing stuff about esp32.
2. The `wifi ssid` and `password` should changed accordingly before you run the test.

---

# How to setup the environment?
1. Windows User: [How to Set Up Raspberry Pi Pico C/C++ Toolchain on Windows with VS Code](https://shawnhymel.com/2096/how-to-set-up-raspberry-pi-pico-c-c-toolchain-on-windows-with-vs-code/)
2. Linux User: [Quick-start your own project](https://github.com/raspberrypi/pico-sdk)
3. OSX User: [Getting started with Pi Pico(Chapter 9.1)](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)


# How to build all the test program
1. Create a new folder named with build, and change directory to build folder. e.g,
```
mkdir build
cd build
```
2. Call cmake and specify the buildings system:
```
# For windows
cmake -G "MinGW Makefiles" ..
# For mac and linux
cmake ..
```
3. Now you can either build all test program with:
```
make -j4
```
or build a single one with:
```
make -j4 test_sht31
```
4. To clean the build before you build again, you would use:
```
make clean
```
# How to flash the device

1. connect the device to your computersss
2. press and hold the BOOT button
3. press the RESET button
4. release the BOOT button
5. copy the _hardware_test.uf2_ file to the device
6. open a serial connection to the device (i.e. via `screen` or `Putty`)


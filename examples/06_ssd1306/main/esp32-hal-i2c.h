// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESP32_HAL_I2C_H_
#define _ESP32_HAL_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include <sys/lock.h>
#include "freertos/semphr.h"
#include <stdint.h>
#include <stdbool.h>
#include "soc/i2c_struct.h"

//#define CONFIG_DISABLE_HAL_LOCKS 1


typedef enum {
    I2C_ERROR_OK,
    I2C_ERROR_DEV,
    I2C_ERROR_ACK,
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUS
} i2c_err_t;

enum {
    I2C_CMD_RSTART,
    I2C_CMD_WRITE,
    I2C_CMD_READ,
    I2C_CMD_STOP,
    I2C_CMD_END
};

struct i2c_struct_t {
    i2c_dev_t * dev;
#if !CONFIG_DISABLE_HAL_LOCKS
    xSemaphoreHandle lock;
#endif
    uint8_t num;
};


//struct i2c_struct_t;
typedef struct i2c_struct_t i2c_t;


i2c_t * i2cInit(uint8_t i2c_num, uint16_t slave_addr, bool addr_10bit_en);

//call this after you setup the bus and pins to send empty packet
//required because when pins are attached, they emit pulses that lock the bus
void i2cInitFix(i2c_t * i2c);

i2c_err_t i2cSetFrequency(i2c_t * i2c, uint32_t clk_speed);
uint32_t i2cGetFrequency(i2c_t * i2c);

i2c_err_t i2cAttachSCL(i2c_t * i2c, int8_t scl);
i2c_err_t i2cDetachSCL(i2c_t * i2c, int8_t scl);
i2c_err_t i2cAttachSDA(i2c_t * i2c, int8_t sda);
i2c_err_t i2cDetachSDA(i2c_t * i2c, int8_t sda);

i2c_err_t i2cWrite(i2c_t * i2c, uint16_t address, bool addr_10bit, uint8_t * data, uint8_t len, bool sendStop);
i2c_err_t i2cRead(i2c_t * i2c, uint16_t address, bool addr_10bit, uint8_t * data, uint8_t len, bool sendStop);


///////// LOW LEVEL DRIVER ACCESS

/* Low level access index = 0 , 1 or 2 */
/*
 * index     - command index (0 to 15)
 * op_code   - is the command
 * byte_num  - This register is to store the amounts of data that is read and written. byte_num in RSTART, STOP, END is null.
 * ack_val   - Each data byte is terminated by an ACK bit used to set the bit level.
 * ack_exp   - This bit is to set an expected ACK value for the transmitter.
 * ack_check - This bit is to decide whether the transmitter checks ACK bit. 1 means yes and 0 means no.
 * */
void i2cSetCmd(i2c_t * i2c, uint8_t index, uint8_t op_code, uint8_t byte_num, bool ack_val, bool ack_exp, bool ack_check);
void i2cResetFiFo(i2c_t * i2c);
void i2cResetCmd(i2c_t * i2c);




#ifdef __cplusplus
}
#endif

#endif /* _ESP32_HAL_I2C_H_ */

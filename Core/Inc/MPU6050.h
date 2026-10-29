/*
 * MPU6050.h
 *
 *  Created on: Oct 27, 2026
 *      Author: mgt
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"

// Function pointer types for I2C hardware abstraction (Interface)
typedef bool (*mpu6050_i2c_write_fn)(I2C_HandleTypeDef *handle, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint8_t len);
typedef bool (*mpu6050_i2c_read_fn)(I2C_HandleTypeDef *handle, uint8_t dev_addr, uint8_t reg_addr, const uint8_t *data, uint8_t len);

// Data structure to hold sensor readings
typedef struct {
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    float temperature;
} mpu6050_data_t;

// Configuration structure
typedef struct {
    uint8_t device_address;
    uint8_t accel_fsr;  // Full scale range
    uint8_t gyro_fsr;   // Full scale range
} mpu6050_config_t;

typedef struct mpu6050_t {
    // Attributes (Private-like data)
	I2C_HandleTypeDef *handle;
    mpu6050_config_t config;
    mpu6050_data_t raw_data;
    bool is_initialized;

    mpu6050_i2c_write_fn i2c_write;
    mpu6050_i2c_read_fn  i2c_read;
} mpu6050_t;

// "public" API
bool mpu6050_init(mpu6050_t *const self,
				const mpu6050_config_t *config,
				I2C_HandleTypeDef *handle,
				mpu6050_i2c_write_fn write,
				mpu6050_i2c_read_fn read);

bool mpu6050_read_sensors(mpu6050_t *const self);

void mpu6050_get_data(const mpu6050_t *const self,
					mpu6050_data_t *out_data);

#endif /* INC_MPU6050_H_ */

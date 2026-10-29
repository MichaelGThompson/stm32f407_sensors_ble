#include "mpu6050.h"

// MPU6050 Register Map (Internal / Private constants)
#define REG_WHO_AM_I         0x75
#define REG_PWR_MGMT_1       0x6B
#define REG_ACCEL_XOUT_H     0x3B
#define WHO_AM_I_IDENTITY	 0x68
#define WHO_AM_I_IDENTITY_2  0x70		// sometimes clone sensors have a different address

// Initializater
bool mpu6050_init(mpu6050_t *const self,
				const mpu6050_config_t *config,
				I2C_HandleTypeDef *handle,
				mpu6050_i2c_write_fn write,
				mpu6050_i2c_read_fn read) {

    if (!self || !config || !write || !read) return false;

    self->handle = handle;
    self->config = *config;
    self->i2c_write = write;
    self->i2c_read = read;
    self->is_initialized = false;

    // Verify connection (Who Am I check)
    uint8_t who_am_i = 0;
    if (!self->i2c_read(self->handle,
    					self->config.device_address,
    					REG_WHO_AM_I,
						&who_am_i,
						1) || (who_am_i != WHO_AM_I_IDENTITY) && (who_am_i != WHO_AM_I_IDENTITY_2)) {
        return false;
    }

    // Wake up MPU6050 (Clear sleep bit)
    uint8_t pwr_mgmt_data = 0x00;
    if (!self->i2c_write(self->handle,
    					self->config.device_address,
						REG_PWR_MGMT_1,
						&pwr_mgmt_data,
						1)) {
        return false;
    }

    self->is_initialized = true;
    return true;
}

// Method to update internal data
bool mpu6050_read_sensors(mpu6050_t *const self) {
    if (!self || !self->is_initialized) return false;

    uint8_t buffer[14]; // 6 bytes Accel, 2 bytes Temp, 6 bytes Gyro

    // Burst read all sensor registers starting at Accel X High
    if (!self->i2c_read(self->handle,
    					self->config.device_address,
						REG_ACCEL_XOUT_H,
						buffer,
						14)) {
        return false;
    }

    // Parse data into object attributes
    self->raw_data.ax = (int16_t)((buffer[0] << 8) | buffer[1]);
    self->raw_data.ay = (int16_t)((buffer[2] << 8) | buffer[3]);
    self->raw_data.az = (int16_t)((buffer[4] << 8) | buffer[5]);

    int16_t raw_temp  = (int16_t)((buffer[6] << 8) | buffer[7]);
    self->raw_data.temperature = ((float)raw_temp / 340.0f) + 36.53f;

    self->raw_data.gx = (int16_t)((buffer[8] << 8)  | buffer[9]);
    self->raw_data.gy = (int16_t)((buffer[10] << 8) | buffer[11]);
    self->raw_data.gz = (int16_t)((buffer[12] << 8) | buffer[13]);

    return true;
}

// Getter method
void mpu6050_get_data(const mpu6050_t *const self, mpu6050_data_t *out_data) {
    if (self && out_data) {
        *out_data = self->raw_data;
    }
}

#include "main.h"
#include "MPU6050.h"

// ---- function pointer implementations for MPU6050
bool mpu6050_platform_i2c_write(I2C_HandleTypeDef *handle,
								uint8_t dev_addr,
								uint8_t reg_addr,
								const uint8_t *data,
								uint8_t len) {

	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(handle,
												dev_addr,
												reg_addr,
												I2C_MEMADD_SIZE_8BIT,
												(uint8_t*)data,
												len,
												HAL_TIMEOUT);
	return status == HAL_OK ? true : false;
}

bool mpu6050_platform_i2c_read(I2C_HandleTypeDef *handle,
							   uint8_t dev_addr,
							   uint8_t reg_addr,
							   const uint8_t *data,
							   uint8_t len) {

	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(handle,
												dev_addr,
												reg_addr,
												I2C_MEMADD_SIZE_8BIT,
												(uint8_t*)data,
												len,
												HAL_TIMEOUT);
	return status == HAL_OK ? true : false;
}

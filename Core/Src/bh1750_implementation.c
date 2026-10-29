#include "main.h"
#include "bh1750.h"

int8_t platform_i2c_write(I2C_HandleTypeDef *handle,
						uint8_t dev_addr,
						const uint8_t *data,
						uint8_t len) {

	HAL_I2C_Master_Transmit(handle,
							dev_addr,
							(uint8_t*)data,
							len,
							HAL_MAX_DELAY);
    return 0; // Return 0 on success
}

int8_t platform_i2c_read(I2C_HandleTypeDef *handle,
						uint8_t dev_addr,
						const uint8_t *data,
						uint8_t len) {
    // Call your target MCU I2C read API here (e.g., HAL_I2C_Master_Receive)
	HAL_I2C_Master_Receive(handle,
						dev_addr,
						(uint8_t*)data,
						len,
						HAL_MAX_DELAY);
    return 0; // Return 0 on success
}

void platform_delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}


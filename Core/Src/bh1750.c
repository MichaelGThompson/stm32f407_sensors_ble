#include "bh1750.h"

#define BH1750_CMD_POWER_DOWN 0x00
#define BH1750_CMD_POWER_ON   0x01
#define BH1750_CMD_RESET      0x07

bh1750_status_t bh1750_init(bh1750_t *self,
							I2C_HandleTypeDef *handle,
							uint8_t i2c_addr,
							bh1750_i2c_write_fn write_fn,
							bh1750_i2c_read_fn read_fn,
							bh1750_delay_ms_fn delay_fn) {
    if (!self || !write_fn || !read_fn || !delay_fn) {
        return BH1750_ERR_PARAM;
    }
    self->handle = handle;
    self->write = write_fn;
    self->read = read_fn;
    self->delay_ms = delay_fn;
    self->i2c_addr = i2c_addr;
    self->lux = 0.0f;
    self->current_mode = BH1750_MODE_CONTINUOUS_HIGH_RES;

    /* Wake up the device */
    uint8_t cmd = BH1750_CMD_POWER_ON;
    if (self->write(self->handle, self->i2c_addr, &cmd, 1) != 0) {
        return BH1750_ERR_I2C;
    }

    /* Default configuration */
    return bh1750_configure_mode(self, self->current_mode);
}

bh1750_status_t bh1750_configure_mode(bh1750_t *self,
									bh1750_mode_t mode) {
    if (!self) return BH1750_ERR_PARAM;

    uint8_t cmd = (uint8_t)mode;
    if (self->write(self->handle, self->i2c_addr, &cmd, 1) != 0) {
        return BH1750_ERR_I2C;
    }

    self->current_mode = mode;

    /* Wait for the conversion startup delay if it's a one-time mode */
    self->delay_ms(180);
    return BH1750_OK;
}

bh1750_status_t bh1750_read_lux(bh1750_t *self) {
    if (!self) return BH1750_ERR_PARAM;

    uint8_t raw_data[2] = {0};

    /* If using one-time mode, re-triggering might be needed depending on timing */
    if (self->read(self->handle, self->i2c_addr, raw_data, 2) != 0) {
        return BH1750_ERR_I2C;
    }

    /* Combine MSB and LSB */
    uint16_t raw_level = (uint16_t)((raw_data[0] << 8) | raw_data[1]);

    /* BH1750 Standard formula: Lux = Raw / 1.2 */
    /* Divide by 1.2 for standard high res; if using High Res 2 mode, divide by 2.4 */
    if (self->current_mode == BH1750_MODE_CONTINUOUS_HIGH_RES2 || self->current_mode == BH1750_MODE_ONE_TIME_HIGH_RES2) {
        self->lux = (float)raw_level / 2.4f;
    } else {
        self->lux = (float)raw_level / 1.2f;
    }

    return BH1750_OK;
}

bh1750_status_t bh1750_power_down(bh1750_t *self) {
    if (!self) return BH1750_ERR_PARAM;

    uint8_t cmd = BH1750_CMD_POWER_DOWN;
    if (self->write(self->handle, self->i2c_addr, &cmd, 1) != 0) {
        return BH1750_ERR_I2C;
    }
    return BH1750_OK;
}

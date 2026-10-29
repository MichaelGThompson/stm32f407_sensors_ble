/*
 * bh1750.h
 *
 *  Created on: Oct 27, 2026
 *      Author: mgt
 */

#ifndef BH1750_H
#define BH1750_H

#include <stdint.h>
#include <stddef.h>
#include "i2c.h"

/* BH1750 I2C Slave Addresses */
#define BH1750_ADDR_LOW   0x23  /* ADDR pin floating or GND */
#define BH1750_ADDR_HIGH  0x5C  /* ADDR pin connected to VCC */

/* BH1750 Operational Modes */
typedef enum {
    BH1750_MODE_CONTINUOUS_HIGH_RES  = 0x10,
    BH1750_MODE_CONTINUOUS_HIGH_RES2 = 0x11,
    BH1750_MODE_CONTINUOUS_LOW_RES   = 0x12,
    BH1750_MODE_ONE_TIME_HIGH_RES    = 0x20,
    BH1750_MODE_ONE_TIME_HIGH_RES2   = 0x21,
    BH1750_MODE_ONE_TIME_LOW_RES    = 0x22
} bh1750_mode_t;

/* Status Codes */
typedef enum {
    BH1750_OK          = 0,
    BH1750_ERR_PARAM   = -1,
    BH1750_ERR_I2C     = -2
} bh1750_status_t;

/* Function pointer types for Hardware Abstraction Layer (HAL) injection */
typedef int8_t (*bh1750_i2c_write_fn)(I2C_HandleTypeDef *handle, uint8_t dev_addr, const uint8_t *data, uint8_t len);
typedef int8_t (*bh1750_i2c_read_fn)(I2C_HandleTypeDef *handle, uint8_t dev_addr, const uint8_t *data, uint8_t len);
typedef void   (*bh1750_delay_ms_fn)(uint32_t ms);

typedef struct {
    /* Hardware Interface Dependencies */
    bh1750_i2c_write_fn write;
    bh1750_i2c_read_fn  read;
    bh1750_delay_ms_fn  delay_ms;

    /* Object Properties (State) */
    I2C_HandleTypeDef 	*handle;
    uint8_t             i2c_addr;
    bh1750_mode_t       current_mode;
    float               lux;
} bh1750_t;

/* API */
bh1750_status_t bh1750_init(bh1750_t *self,
							I2C_HandleTypeDef *handle,
							uint8_t i2c_addr,
							bh1750_i2c_write_fn write_fn,
							bh1750_i2c_read_fn read_fn,
							bh1750_delay_ms_fn delay_fn);
bh1750_status_t bh1750_configure_mode(bh1750_t *self, bh1750_mode_t mode);
bh1750_status_t bh1750_read_lux(bh1750_t *self);
bh1750_status_t bh1750_power_down(bh1750_t *self);

#endif /* BH1750_H */


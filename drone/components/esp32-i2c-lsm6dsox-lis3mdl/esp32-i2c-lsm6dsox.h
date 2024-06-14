#ifndef __ESP32_I2C_LSM6DSOX_H_
#define __ESP32_I2C_LSM6DSOX_H_

#include "driver/i2c_master.h"


#define CTRL1_XL 0x10 // datasheet page 44
#define CTRL2_G 0x11 // ^
#define CTRL3_C 0x12 // ^
#define CTRL4_C 0x13 // ^
#define CTRL5_C 0x14 // ^
#define CTRL6_C 0x15 // ^
#define CTRL7_G 0x16 // ^
#define CTRL8_XL 0x17 // ^
#define CTRL9_XL 0x18 // ^
#define CTRL10_C 0x19 // ^
#define OUTX_L_G 0x22 // datasheet page 45
#define OUTX_H_G 0x23 // ^
#define OUTY_L_G 0x24 // ^
#define OUTY_H_G 0x25 // ^
#define OUTZ_L_G 0x26 // ^
#define OUTZ_H_G 0x27 // ^
#define OUTX_L_A 0x28 // ^
#define OUTX_H_A 0x29 // ^
#define OUTY_L_A 0x2A // ^
#define OUTY_H_A 0x2B // ^
#define OUTZ_L_A 0x2C // ^
#define OUTZ_H_A 0x2D // ^
#define LSM6DSOX_ACC_SENSITIVITY_FS_2G  0.061f // datasheet page 10
#define LSM6DSOX_ACC_SENSITIVITY_FS_4G  0.122f // ^
#define LSM6DSOX_ACC_SENSITIVITY_FS_8G  0.244f // ^
#define LSM6DSOX_ACC_SENSITIVITY_FS_16G 0.488f // ^
#define LSM6DSOX_GYRO_SENSITIVITY_FS_125DPS  4.375f // ^
#define LSM6DSOX_GYRO_SENSITIVITY_FS_250DPS  8.750f // ^
#define LSM6DSOX_GYRO_SENSITIVITY_FS_500DPS  17.500f // ^
#define LSM6DSOX_GYRO_SENSITIVITY_FS_1000DPS 35.000f // ^
#define LSM6DSOX_GYRO_SENSITIVITY_FS_2000DPS 70.000f // ^


struct lsm6dsox_ctrl1_xl {
	uint8_t not_used_01:1;
	uint8_t lpf2_xl_en:1;
	uint8_t fs_xl:2;
	uint8_t odr_xl:4;
};

typedef enum {
    LSM6DSOX_XL_ODR_OFF    = 0,
    LSM6DSOX_XL_ODR_12Hz5  = 1,
    LSM6DSOX_XL_ODR_26Hz   = 2,
    LSM6DSOX_XL_ODR_52Hz   = 3,
    LSM6DSOX_XL_ODR_104Hz  = 4,
    LSM6DSOX_XL_ODR_208Hz  = 5,
    LSM6DSOX_XL_ODR_417Hz  = 6,
    LSM6DSOX_XL_ODR_833Hz  = 7,
    LSM6DSOX_XL_ODR_1667Hz = 8,
    LSM6DSOX_XL_ODR_3333Hz = 9,
    LSM6DSOX_XL_ODR_6667Hz = 10,
    LSM6DSOX_XL_ODR_1Hz6   = 11, /* (low power only) */
} lsm6dsox_odr_xl_t;

typedef enum {
	LSM6DSOX_FS_XL_00 = 0, // datasheet page 56
	LSM6DSOX_FS_XL_01 = 1, // ^
	LSM6DSOX_FS_XL_10 = 2, // ^
	LSM6DSOX_FS_XL_11 = 3, // ^
} lsm6dsox_fs_xl_t;

struct lsm6dsox_ctrl2_g {
    uint8_t not_used_01:1;
    uint8_t fs_125:1;
    uint8_t fs_g:2;
    uint8_t odr_g:4;
};

typedef enum {
    LSM6DSOX_GY_ODR_OFF    = 0,
    LSM6DSOX_GY_ODR_12Hz5  = 1,
    LSM6DSOX_GY_ODR_26Hz   = 2,
    LSM6DSOX_GY_ODR_52Hz   = 3,
    LSM6DSOX_GY_ODR_104Hz  = 4,
    LSM6DSOX_GY_ODR_208Hz  = 5,
    LSM6DSOX_GY_ODR_417Hz  = 6,
    LSM6DSOX_GY_ODR_833Hz  = 7,
    LSM6DSOX_GY_ODR_1667Hz = 8,
    LSM6DSOX_GY_ODR_3333Hz = 9,
    LSM6DSOX_GY_ODR_6667Hz = 10,
} lsm6dsox_odr_g_t;

typedef enum {
    LSM6DSOX_FS_G_00 = 0, // datasheet page 57
    LSM6DSOX_FS_G_01 = 1, // ^
    LSM6DSOX_FS_G_10 = 2, // ^
    LSM6DSOX_FS_G_11 = 3, // ^
} lsm6dsox_fs_g_t;


struct lsm6dsox_ctrl6_c {
    uint8_t ftype:3;
    uint8_t usr_off_w:1;
    uint8_t xl_hm_mode:1;
    uint8_t den_mode:3; /* trig_en + lvl1_en + lvl2_en */
};


struct lsm6dsox_ctrl7_g {
    uint8_t ois_on:1;
    uint8_t usr_off_on_out:1;
    uint8_t ois_on_en:1;
    uint8_t not_used_01:1;
    uint8_t hpm_g:2;
    uint8_t hp_en_g:1;
    uint8_t g_hm_mode:1;
};


struct lsm6dsox_ctrl8_xl {
    uint8_t low_pass_on_6d:1;
    uint8_t xl_fs_mode:1;
    uint8_t hp_slope_xl_en:1;
    uint8_t fastsettl_mode_xl:1;
    uint8_t hp_ref_mode_xl:1;
    uint8_t hpcf_xl:3;
};


struct i2c_lsm6dsox {
    i2c_master_dev_handle_t *i2c_handle;
	float accelerometer_sensitivity;
	float gyroscope_sensitivity;
};


void esp_i2c_lsm6dsox_begin(struct i2c_lsm6dsox *i2c_lsm6dsox);

void esp_i2c_lsm6dsox_get_gyro_data(struct i2c_lsm6dsox *i2c_lsm6dsox, float *outxyz_g);

float esp_i2c_lsm6dsox_get_gyro_x(struct i2c_lsm6dsox *i2c_lsm6dsox);

float esp_i2c_lsm6dsox_get_gyro_y(struct i2c_lsm6dsox *i2c_lsm6dsox);

float esp_i2c_lsm6dsox_get_gyro_z(struct i2c_lsm6dsox *i2c_lsm6dsox);

void esp_i2c_lsm6dsox_get_accel_data(struct i2c_lsm6dsox *i2c_lsm6dsox, float *outxyz_a);

float esp_i2c_lsm6dsox_get_accel_x(struct i2c_lsm6dsox *i2c_lsm6dsox);

float esp_i2c_lsm6dsox_get_accel_y(struct i2c_lsm6dsox *i2c_lsm6dsox);

float esp_i2c_lsm6dsox_get_accel_z(struct i2c_lsm6dsox *i2c_lsm6dsox);


#endif

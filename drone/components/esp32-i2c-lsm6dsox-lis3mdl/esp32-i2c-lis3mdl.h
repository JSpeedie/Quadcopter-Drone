#ifndef __ESP32_I2C_LIS3MDL_H_
#define __ESP32_I2C_LIS3MDL_H_

#include "driver/i2c_master.h"


#define CTRL_REG1 0x20 // datasheet page 18
#define CTRL_REG2 0x21 // ^
#define CTRL_REG3 0x22 // ^
#define CTRL_REG4 0x23 // ^
#define CTRL_REG5 0x24 // ^
#define OUTX_L 0x28 // ^
#define OUTX_H 0x29 // ^
#define OUTY_L 0x2A // ^
#define OUTY_H 0x2B // ^
#define OUTZ_L 0x2C // ^
#define OUTZ_H 0x2D // ^
#define LIS3MDL_SENSITIVITY_FS_4GAUSS 6842.0f // datasheet page 4
#define LIS3MDL_SENSITIVITY_FS_8GAUSS 3421.0f // ^
#define LIS3MDL_SENSITIVITY_FS_12GAUSS 2281.0f // ^
#define LIS3MDL_SENSITIVITY_FS_16GAUSS 1711.0f // ^


struct lis3mdl_ctrl_reg1 {
    uint8_t st:1;
    uint8_t fast_odr:1;
    uint8_t do_bits:3;
    uint8_t om:2;
    uint8_t temp_en:1;
};

typedef enum {
    LIS3MDL_OM_LOWPOWER             = 0,
    LIS3MDL_OM_MEDIUMPERFORMANCE    = 1,
    LIS3MDL_OM_HIGHPERFORMANCE      = 2,
    LIS3MDL_OM_ULTRAHIGHPERFORMANCE = 3,
} lis3mdl_om_t;


struct lis3mdl_ctrl_reg2 {
    uint8_t not_used1:2;
    uint8_t soft_rst:1;
    uint8_t reboot:1;
    uint8_t not_used2:1;
    uint8_t fs:2;
    uint8_t not_used3:1;
};

typedef enum {
    LIS3MDL_FS_4GAUSS  = 0,
    LIS3MDL_FS_8GAUSS  = 1,
    LIS3MDL_FS_12GAUSS = 2,
    LIS3MDL_FS_16GAUSS = 3,
} lis3mdl_fs_t;


struct lis3mdl_ctrl_reg3 {
    uint8_t md:2;
    uint8_t sim:1;
    uint8_t not_used1:2;
    uint8_t lp:1;
    uint8_t not_used2:2;
};

typedef enum {
    LIS3MDL_MD_CONTINUOUSCONVERSION = 0,
    LIS3MDL_MD_SINGLECONVERSION     = 1, /* Must be used when sampling frequency >= 0.625 Hz, <= 80 Hz */
    LIS3MDL_MD_POWERDOWN1           = 2,
    LIS3MDL_MD_POWERDOWN2           = 3,
} lis3mdl_md_t;


struct lis3mdl_ctrl_reg4 {
    uint8_t not_used1:1;
    uint8_t ble:1;
    uint8_t omz:2;
    uint8_t not_used2:4;
};

typedef enum {
    LIS3MDL_OMZ_LOWPOWER             = 0,
    LIS3MDL_OMZ_MEDIUMPERFORMANCE    = 1,
    LIS3MDL_OMZ_HIGHPERFORMANCE      = 2,
    LIS3MDL_OMZ_ULTRAHIGHPERFORMANCE = 3,
} lis3mdl_omz_t;


struct i2c_lis3mdl {
    i2c_master_dev_handle_t *i2c_handle;
	float sensitivity;
};


void esp_i2c_lis3mdl_begin(struct i2c_lis3mdl *i2c_lis3mdl);

void esp_i2c_lis3mdl_get_data(struct i2c_lis3mdl *i2c_lis3mdl, float *outxyz);

float esp_i2c_lis3mdl_get_x(struct i2c_lis3mdl *i2c_lis3mdl);

float esp_i2c_lis3mdl_get_y(struct i2c_lis3mdl *i2c_lis3mdl);

float esp_i2c_lis3mdl_get_z(struct i2c_lis3mdl *i2c_lis3mdl);


#endif

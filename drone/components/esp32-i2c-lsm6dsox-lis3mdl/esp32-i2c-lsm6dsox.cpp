#include <inttypes.h>

#include "driver/i2c_master.h"

#include "esp32-i2c-lsm6dsox.h"
#include "esp32-i2c-lsm6dsox-lis3mdl-common.h"


/** Takes a struct i2c_lsm6dsox and sets up the device with set defaults,
 * bringing the device to a point where we can start reading data from it.
 *
 * Note: this function requires that the 'i2c_lsm6dsox' argument has
 * its 'i2c_handle' member set correctly with an 'i2c_master_dev_handle_t' that
 * has been added to the i2c master bus using 'i2c_master_bus_add_device()'.
 */
void esp_i2c_lsm6dsox_begin(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* {{{ */
    /* 1. Set accelerometer to ON and the specific output data rate */
    /* 1a. Read the current value for the CTRL1_XL register */
    uint8_t buf = CTRL1_XL;
    struct lsm6dsox_ctrl1_xl ctrl1_xl_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl1_xl_content, 1, -1));
    /* 1b. Set the ODR bits of the CTRL1_XL register (datasheet page 56) */
	ctrl1_xl_content.odr_xl = LSM6DSOX_XL_ODR_12Hz5;
    ctrl1_xl_content.lpf2_xl_en = 1;
    /* 1c. Write the new value back to the CTRL1_XL register */
    uint8_t sub_and_data[2];
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl1_xl_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lsm6dsox->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));

    /* 2. Turn on high-performance mode for the accelerometer */
    /* 2a. Read the current value for the CTRL6_C register */
    buf = CTRL6_C;
    struct lsm6dsox_ctrl6_c ctrl6_c_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl6_c_content, 1, -1));
    /* 2b. Enable high-performance mode for the accelerometer (datasheet page 61) */
	ctrl6_c_content.xl_hm_mode = 1;
    /* 2c. Write the new contents back to the register */
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl6_c_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lsm6dsox->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));

    /* 3. Set gyroscope to ON and at the specific output data rate */
    /* 3a. Read the current value for the CTRL2_G register */
    buf = CTRL2_G;
    struct lsm6dsox_ctrl2_g ctrl2_g_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl2_g_content, 1, -1));
    /* 3b. Set the ODR bits of the CTRL2_G register (datasheet page 56) */
	ctrl2_g_content.odr_g = LSM6DSOX_GY_ODR_12Hz5;
    /* 3c. Write the new value back to the CTRL2_G register */
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl2_g_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lsm6dsox->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));

    /* 4. Turn on high-performance mode for the gyroscope */
    /* 4a. Read the current value for the CTRL7_G register */
    buf = CTRL7_G;
    struct lsm6dsox_ctrl7_g ctrl7_g_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl7_g_content, 1, -1));
    /* 4b. Enable high-performance mode for the gyroscope (datasheet page 62) */
	ctrl7_g_content.g_hm_mode = 1;
    /* 4c. Write the new contents back to the register */
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl7_g_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lsm6dsox->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));

    /* 5. Get and store sensor sensitivities */
    /* 5a. Read CTRL8_XL register to get XL_FS_MODE in order to determine
     * accelerometer sensitivity */
    buf = CTRL8_XL;
    struct lsm6dsox_ctrl8_xl ctrl8_xl_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl8_xl_content, 1, -1));

    /* 5b. Set the accelerometer sensitivity multiplier for the LSM6DSOX
     * (datasheet page 56) */
    switch (ctrl1_xl_content.fs_xl) {
        case LSM6DSOX_FS_XL_00:
            i2c_lsm6dsox->accelerometer_sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_2G;
            break;
        case LSM6DSOX_FS_XL_01:
            i2c_lsm6dsox->accelerometer_sensitivity = (ctrl8_xl_content.xl_fs_mode == 0) ? LSM6DSOX_ACC_SENSITIVITY_FS_16G : LSM6DSOX_ACC_SENSITIVITY_FS_2G;
            break;
        case LSM6DSOX_FS_XL_10:
            i2c_lsm6dsox->accelerometer_sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_4G;
            break;
        case LSM6DSOX_FS_XL_11:
            i2c_lsm6dsox->accelerometer_sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_8G;
            break;
        default:
            i2c_lsm6dsox->accelerometer_sensitivity = LSM6DSOX_ACC_SENSITIVITY_FS_2G;
    }

    /* 5c. Set the gyroscope sensitivity multiplier for the LSM6DSOX. Remember
     * (datasheet page 57) */
    switch (ctrl2_g_content.fs_125) {
        case 1:
            i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_125DPS;
            break;
        case 0:
            switch (ctrl2_g_content.fs_g) {
                case LSM6DSOX_FS_G_00:
                    i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_250DPS;
                    break;
                case LSM6DSOX_FS_G_01:
                    i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_500DPS;
                    break;
                case LSM6DSOX_FS_G_10:
                    i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_1000DPS;
                    break;
                case LSM6DSOX_FS_G_11:
                    i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_2000DPS;
                    break;
                default:
                    i2c_lsm6dsox->gyroscope_sensitivity = LSM6DSOX_GYRO_SENSITIVITY_FS_250DPS;
            }
            break;
    }
    /* }}} */
}


/* Takes a 3 element array of uint16_t's because historically floats have been
 * 32 bit, but the data on the gyro is represented as a 16 bit float */
void esp_i2c_lsm6dsox_get_gyro_data(struct i2c_lsm6dsox *i2c_lsm6dsox, \
    float *outxyz_g) {

    /* Get both the lower 8 bits and the higher 8 bits for x, y, and z by
     * requesting the lower 8 bits for the x axis data, but reading for 6 bytes */
    uint8_t buf = OUTX_L_G;
    union threeaxes outxyz_g_raw;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)outxyz_g_raw.u16, 6, -1));

    outxyz_g[0] = ((float) outxyz_g_raw.i16[0]) * i2c_lsm6dsox->gyroscope_sensitivity;
    outxyz_g[1] = ((float) outxyz_g_raw.i16[1]) * i2c_lsm6dsox->gyroscope_sensitivity;
    outxyz_g[2] = ((float) outxyz_g_raw.i16[2]) * i2c_lsm6dsox->gyroscope_sensitivity;

    return;
}


float esp_i2c_lsm6dsox_get_gyro_x(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTX_L_G;
    uint16_t outx_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outx_g, 2, -1));

    float ret = ((float) outx_g) * i2c_lsm6dsox->gyroscope_sensitivity;

    return ret;
}


float esp_i2c_lsm6dsox_get_gyro_y(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTY_L_G;
    uint16_t outy_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outy_g, 2, -1));

    float ret = ((float) outy_g) * i2c_lsm6dsox->gyroscope_sensitivity;

    return ret;
}


float esp_i2c_lsm6dsox_get_gyro_z(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTZ_L_G;
    uint16_t outz_g;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outz_g, 2, -1));

    float ret = ((float) outz_g) * i2c_lsm6dsox->gyroscope_sensitivity;

    return ret;
}


void esp_i2c_lsm6dsox_get_accel_data(struct i2c_lsm6dsox *i2c_lsm6dsox, \
    float *outxyz_a) {

    /* Get both the lower 8 bits and the higher 8 bits for x, y, and z by
     * requesting the lower 8 bits for the x axis data, but reading for 6 bytes */
    uint8_t buf = OUTX_L_A;
    union threeaxes outxyz_a_raw;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outxyz_a_raw.u16, 6, -1));

    outxyz_a[0] = ((float) outxyz_a_raw.i16[0]) * i2c_lsm6dsox->accelerometer_sensitivity;
    outxyz_a[1] = ((float) outxyz_a_raw.i16[1]) * i2c_lsm6dsox->accelerometer_sensitivity;
    outxyz_a[2] = ((float) outxyz_a_raw.i16[2]) * i2c_lsm6dsox->accelerometer_sensitivity;

    return;
}


float esp_i2c_lsm6dsox_get_accel_x(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTX_L_A;
    int16_t outx_a = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outx_a, 2, -1));

    float ret = ((float) outx_a) * i2c_lsm6dsox->accelerometer_sensitivity;

    return ret;
}


float esp_i2c_lsm6dsox_get_accel_y(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTY_L_A;
    int16_t outy_a = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outy_a, 2, -1));

    float ret = ((float) outy_a) * i2c_lsm6dsox->accelerometer_sensitivity;

    return ret;
}


float esp_i2c_lsm6dsox_get_accel_z(struct i2c_lsm6dsox *i2c_lsm6dsox) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTZ_L_A;
    int16_t outz_a = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lsm6dsox->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outz_a, 2, -1));

    float ret = ((float) outz_a) * i2c_lsm6dsox->accelerometer_sensitivity;

    return ret;
}

#include <inttypes.h>

#include "driver/i2c_master.h"

#include "esp32-i2c-lis3mdl.h"
#include "esp32-i2c-lsm6dsox-lis3mdl-common.h"


/** Takes a struct i2c_lis3mdl and sets up the device with set defaults,
 * bringing the device to a point where we can start reading data from it.
 *
 * Note: this function requires that the 'i2c_lis3mdl' argument has
 * its 'i2c_handle' member set correctly with an 'i2c_master_dev_handle_t' that
 * has been added to the i2c master bus using 'i2c_master_bus_add_device()'.
 */
void esp_i2c_lis3mdl_begin(struct i2c_lis3mdl *i2c_lis3mdl) {
    /* {{{ */
    printf("starting lis3mdl initialization (1)\n");
    /* 1. Set X and Y axes to high-performance mode, set fast ODR on */
    /* 1a. Read the current value for the CTRL_REG1 register */
    uint8_t buf = CTRL_REG1;
    struct lis3mdl_ctrl_reg1 ctrl_reg1_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl_reg1_content, 1, -1));
    /* 1b. Set the OM bits of the CTRL_REG1 register (datasheet page 20) */
	ctrl_reg1_content.om = LIS3MDL_OM_HIGHPERFORMANCE;
    /* 1c. Enable FAST_ODR in the CTRL_REG1 register (datasheet page 20) */
	ctrl_reg1_content.fast_odr = 1;
    /* 1d. Write the new value back to the CTRL_REG1 register */
    uint8_t sub_and_data[2];
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl_reg1_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lis3mdl->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));
    printf("lis3mdl initialization (2)\n");

    /* 2. Set Z axis to high-performance mode */
    /* 2a. Read the current value for the CTRL_REG4 register */
    buf = CTRL_REG4;
    struct lis3mdl_ctrl_reg4 ctrl_reg4_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl_reg4_content, 1, -1));
    /* 2b. Set the OMZ bits of the CTRL_REG4 register (datasheet page 22) */
	ctrl_reg4_content.omz = LIS3MDL_OMZ_HIGHPERFORMANCE;
    /* 2c. Write the new value back to the CTRL_REG4 register */
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl_reg4_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lis3mdl->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));
    printf("lis3mdl initialization (3)\n");

    /* 3. Set system operating mode to continuous conversion */
    /* 3a. Read the current value for the CTRL_REG3 register */
    buf = CTRL_REG3;
    struct lis3mdl_ctrl_reg3 ctrl_reg3_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl_reg3_content, 1, -1));
    /* 3b. Set the MD bits of the CTRL_REG3 register (datasheet page 21) */
	ctrl_reg3_content.md = LIS3MDL_MD_CONTINUOUSCONVERSION;
    /* 3c. Write the new value back to the CTRL_REG3 register */
    sub_and_data[0] = buf;
    sub_and_data[1] = *((uint8_t *) &ctrl_reg3_content);
    ESP_ERROR_CHECK(i2c_master_transmit(*(i2c_lis3mdl->i2c_handle), &sub_and_data[0], \
        sizeof(sub_and_data), -1));
    printf("lis3mdl initialization (4)\n");

    /* 4. Get and store sensor sensitivities */
    /* 4a. Read CTRL_REG2 register to get FS in order to determine the
     * magnetometer sensitivity */
    buf = CTRL_REG2;
    struct lis3mdl_ctrl_reg2 ctrl_reg2_content;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&ctrl_reg2_content, 1, -1));
    printf("lis3mdl initialization (5)\n");

    /* 4b. Set the sensitivity multiplier for the LIS3MDL (datasheet page 21) */
    switch (ctrl_reg2_content.fs) {
        case LIS3MDL_FS_4GAUSS:
            i2c_lis3mdl->sensitivity = LIS3MDL_SENSITIVITY_FS_4GAUSS;
            break;
        case LIS3MDL_FS_8GAUSS:
            i2c_lis3mdl->sensitivity = LIS3MDL_SENSITIVITY_FS_8GAUSS;
            break;
        case LIS3MDL_FS_12GAUSS:
            i2c_lis3mdl->sensitivity = LIS3MDL_SENSITIVITY_FS_12GAUSS;
            break;
        case LIS3MDL_FS_16GAUSS:
            i2c_lis3mdl->sensitivity = LIS3MDL_SENSITIVITY_FS_16GAUSS;
            break;
        default:
            i2c_lis3mdl->sensitivity = LIS3MDL_SENSITIVITY_FS_4GAUSS;
    }
    printf("lis3mdl initialization (6)\n");
    /* }}} */
}


void esp_i2c_lis3mdl_get_data(struct i2c_lis3mdl *i2c_lis3mdl, \
    float *outxyz) {

    /* Get both the lower 8 bits and the higher 8 bits for x, y, and z by
     * requesting the lower 8 bits for the x axis data, but reading for 6 bytes */
    uint8_t buf = OUTX_L;
    union threeaxes outxyz_raw;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outxyz_raw.u16, 6, -1));

    outxyz[0] = ((float) outxyz_raw.i16[0]) / i2c_lis3mdl->sensitivity;
    outxyz[1] = ((float) outxyz_raw.i16[1]) / i2c_lis3mdl->sensitivity;
    outxyz[2] = ((float) outxyz_raw.i16[2]) / i2c_lis3mdl->sensitivity;

    return;
}


float esp_i2c_lis3mdl_get_x(struct i2c_lis3mdl *i2c_lis3mdl) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTX_L;
    uint16_t outx;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outx, 2, -1));

    float ret = ((float) outx) / i2c_lis3mdl->sensitivity;

    return ret;
}


float esp_i2c_lis3mdl_get_y(struct i2c_lis3mdl *i2c_lis3mdl) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTY_L;
    uint16_t outy;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outy, 2, -1));

    float ret = ((float) outy) / i2c_lis3mdl->sensitivity;

    return ret;
}


float esp_i2c_lis3mdl_get_z(struct i2c_lis3mdl *i2c_lis3mdl) {
    /* Get both the lower 8 bits and the higher 8 bits from their respective
     * registers by requesting the lower 8 bits, but reading for 2 bytes */
    uint8_t buf = OUTZ_L;
    uint16_t outz;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(*(i2c_lis3mdl->i2c_handle), &buf, \
        sizeof(buf), (uint8_t *)&outz, 2, -1));

    float ret = ((float) outz) / i2c_lis3mdl->sensitivity;

    return ret;
}

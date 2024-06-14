#ifndef __ESP32_I2C_LSM6DSOX_LIS3MDL_COMMON_H_
#define __ESP32_I2C_LSM6DSOX_LIS3MDL_COMMON_H_

/* A union which we use for reinterpreting data. We can read data into the
 * 'u16' element as an unsigned int if required, but then interpret the 0s and
 * 1s that comprised 'u16' as a signed integer through accessing the 'i16'
 * element of the union. */
union threeaxes {
    int16_t i16[3];
    uint16_t u16[3];
};


#endif

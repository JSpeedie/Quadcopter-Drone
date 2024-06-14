#include <math.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_hidh_api.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/spi_master.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "drone.h"

/* Component includes */
#include "esp32-i2c-lsm6dsox.h"
#include "esp32-i2c-lis3mdl.h"


/* I2C Defines {{{ */
#define I2C_BUS_PORT 0
/* Going off  https://learn.adafruit.com/assets/111179 */
#define I2C_SDA_PIN_NUM 23
#define I2C_SCL_PIN_NUM 22
/* }}} */


SemaphoreHandle_t dof_data_semaphore = NULL;
i2c_master_dev_handle_t *magnetometer_handle;
i2c_master_dev_handle_t *accelgyro_handle;
struct i2c_lsm6dsox *i2c_lsm6dsox;
struct i2c_lis3mdl *i2c_lis3mdl;
struct dof_data dof_data;

struct drone_state drone_state = {
    .pitch = 0.0,
    .roll = 0.0,
};
double tick_period_s;


void get_rc_data(void *arg) {

    /* Set the frequency of the loop in this function to 3 ticks */
    const TickType_t taskFrequency = 3;
    TickType_t lastWakeTime;

    while (1) {
        /* Update the lastWakeTime variable to have the current time */
        lastWakeTime = xTaskGetTickCount();

        /* Update the stored location of the ball */
        if (xSemaphoreTake(dof_data_semaphore, portMAX_DELAY) == pdTRUE) {
            ball_p_x = drone_state.ball_p_x;
            ball_p_y = drone_state.ball_p_y;

            xSemaphoreGive(dof_data_semaphore);
        }

        /* Delay such that this loop executes every 'taskFrequency' ticks */
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}


/** Take a struct containing both pointers to where the 9 DOF sensor data
 * is stored (so it can update it) and game state data so it can adjust
 * it as well */
void get_9dof_data(void *arg) {
    /* Set the frequency of the loop in this function to 2 ticks */
    const TickType_t taskFrequency = 3;
    TickType_t lastWakeTime;

	/* I2C 9 DOF Initialization {{{ */
    /* 1. Configure the i2c master bus */
	i2c_master_bus_config_t i2c_mst_config = {
		.clk_source = I2C_CLK_SRC_DEFAULT,
		.i2c_port = I2C_BUS_PORT,
		.scl_io_num = I2C_SCL_PIN_NUM,
		.sda_io_num = I2C_SDA_PIN_NUM,
		.glitch_ignore_cnt = 7,
		.flags.enable_internal_pullup = false
	};

	i2c_master_bus_handle_t bus_handle;
	ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

	/* 2. Configure the LIS3MDL (magnetometer) */
	i2c_device_config_t magnetometer_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x1E,
		.scl_speed_hz = 100000,
	};

	magnetometer_handle = malloc(sizeof(i2c_master_dev_handle_t));
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &magnetometer_cfg, \
        magnetometer_handle));

	/* 3. Configure the LSM6DSOX (accelerometer + gyroscope) */
	i2c_device_config_t accelgyro_cfg = {
		.dev_addr_length = I2C_ADDR_BIT_LEN_7,
		.device_address = 0x6A,
		.scl_speed_hz = 100000,
	};

	accelgyro_handle = malloc(sizeof(i2c_master_dev_handle_t));
	ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &accelgyro_cfg, \
        accelgyro_handle));
    printf("about to initialize i2c devs\n");

    /* 4a. Turn on and set operation control for accelerometer and gyro */
    i2c_lsm6dsox = malloc(sizeof(struct i2c_lsm6dsox));
    i2c_lsm6dsox->i2c_handle = accelgyro_handle;
    esp_i2c_lsm6dsox_begin(i2c_lsm6dsox);
    printf("I2C lsm6dsox initialized\n");
    /* 4b. Turn on and set operation control for magnetometer */
    i2c_lis3mdl = malloc(sizeof(struct i2c_lis3mdl));
    i2c_lis3mdl->i2c_handle = magnetometer_handle;
    /* esp_i2c_lis3mdl_begin(i2c_lis3mdl); */
    printf("I2C lis3mdl initialized\n");
    /* }}} */

    printf("About to start data loop\n");

    while (1) {
        /* Update the lastWakeTime variable to have the current time */
        lastWakeTime = xTaskGetTickCount();

        /* esp_i2c_lsm6dsox_get_gyro_data(i2c_lsm6dsox, dof_data.g_xyz); */
        /* /1* float g_x = esp_i2c_lsm6dsox_get_gyro_x(i2c_lsm6dsox); *1/ */
        /* /1* float g_y = esp_i2c_lsm6dsox_get_gyro_y(i2c_lsm6dsox); *1/ */
        /* /1* float g_z = esp_i2c_lsm6dsox_get_gyro_z(i2c_lsm6dsox); *1/ */
        /* /1* Convert from mdps (millidegrees per second) to dps (degrees per second) *1/ */
        /* dof_data.g_xyz[0] /= 1000; */
        /* dof_data.g_xyz[1] /= 1000; */
        /* dof_data.g_xyz[2] /= 1000; */

        esp_i2c_lsm6dsox_get_accel_data(i2c_lsm6dsox, dof_data.a_xyz);
        /* Convert from mg (milligravity, not milligrams) to g (gravity)
            * -> /= 1000
            * Convert from g to m/s^2 (on earth at sea leavel)
            * -> *= 9.81
            * Combining both operations
            * -> /= (1000 / 9.81) -> /= 101.94 */
        drone_state.ball_a_x = dof_data.a_xyz[0] / 101.94;
        drone_state.ball_a_y = dof_data.a_xyz[1] / 101.94;
        drone_state.ball_a_z = dof_data.a_xyz[2] / 101.94;
        /* double acc_z = dof_data.a_xyz[2] / 101.94; */

        /* esp_i2c_lis3mdl_get_data(i2c_lis3mdl, t->dof_data.m_xyz); */


        /* For my scenario I decided that:
            * pitch = rotation around the x axis (will affect the y coord)
            * roll = rotation around the y axis (will affect the x coord)
            */

        // TODO: problem?: these calculations don't generate NaN's for the
        // pitch and roll, but they don't differentiate between 85 deg and
        // what would be 95 deg - they both just end up as 85
        drone_state.pitch = \
            atan(drone_state.ball_a_y / sqrt(drone_state.ball_a_x * drone_state.ball_a_x + acc_z * acc_z));
        drone_state.roll = \
            atan(drone_state.ball_a_x / sqrt(drone_state.ball_a_y * drone_state.ball_a_y + acc_z * acc_z));
		// TODO: there's a problem with these derivations: one of them gives
		// NaN when the IMU is close to 90 degrees rotated around one axis
        /* drone_state.pitch = atan2(drone_state.ball_a_y, acc_z); */
        /* drone_state.roll = asin(drone_state.ball_a_x / gravity_constant); */

        /* printf("pitch: (% #3.2lf°)    roll: (% #3.2lf°)\n", \ */
        /*     drone_state.pitch * 57.2958, drone_state.roll * 57.2958); */

        if (xSemaphoreTake(dof_data_semaphore, portMAX_DELAY) == pdTRUE) {
            drone_state.ball_p_x += drone_state.ball_v_x / pixel_width;
            drone_state.ball_p_y += drone_state.ball_v_y / pixel_width;

            xSemaphoreGive(dof_data_semaphore);
        }

        /* Delay such that this loop executes every 'taskFrequency' ticks */
        vTaskDelayUntil(&lastWakeTime, taskFrequency);
    }
}


void app_main(void) {
    /* This is used to adjust physics calculations performed using m/s or m/s^2
     * for our data which will be tied to the tick rate of the ESP32 and
     * the frequency of the task in ticks */
    tick_period_s = portTICK_PERIOD_MS / 1000.0;

    dof_data.g_xyz = malloc(sizeof(float) * 3);
    dof_data.g_xyz[0] = 0.0f;
    dof_data.g_xyz[1] = 0.0f;
    dof_data.g_xyz[2] = 0.0f;

    dof_data.a_xyz = malloc(sizeof(float) * 3);
    dof_data.a_xyz[0] = 0.0f;
    dof_data.a_xyz[1] = 0.0f;
    dof_data.a_xyz[2] = 0.0f;

    dof_data.m_xyz = malloc(sizeof(float) * 3);
    dof_data.m_xyz[0] = 0.0f;
    dof_data.m_xyz[1] = 0.0f;
    dof_data.m_xyz[2] = 0.0f;

    vSemaphoreCreateBinary(dof_data_semaphore);
    if (dof_data_semaphore == NULL) {
        printf("ERROR: creating semaphore!\n");
    }

    TaskHandle_t get_rc_data_task;
    TaskHandle_t get_9dof_data_task;

    xTaskCreatePinnedToCore(get_rc_data, "get_rc_data", 20480, \
        (void *)NULL, 10, &get_rc_data_task, 0);
    xTaskCreatePinnedToCore(get_9dof_data, "get_9dof_data", 20480, \
        (void *)NULL, 10, &get_9dof_data_task, 1);
}

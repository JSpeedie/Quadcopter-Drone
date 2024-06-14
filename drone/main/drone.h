#ifndef __DRONE_H_
#define __DRONE_H_

#include <inttypes.h>

#include "freertos/semphr.h"

#include "esp32-i2c-lsm6dsox.h"
#include "esp32-i2c-lis3mdl.h"


struct dof_data {
	float *g_xyz;
	float *a_xyz;
	float *m_xyz;
};

struct drone_state {
	double pitch; /* These are stored in Radians */
	double roll;
};


#endif

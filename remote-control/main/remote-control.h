#ifndef __REMOTE_CONTROL_H_
#define __REMOTE_CONTROL_H_

#include <inttypes.h>

#include "freertos/semphr.h"

#include "esp32-i2c-lsm6dsox.h"
#include "esp32-i2c-lis3mdl.h"


struct controller_data {
	double x; /* These are stored in Radians */
	double y;
};


#endif

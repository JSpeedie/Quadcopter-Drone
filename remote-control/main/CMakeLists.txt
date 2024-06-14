set(srcs "remote-control.cpp")

idf_component_register(SRCS "${srcs}"
                    REQUIRES driver
                    REQUIRES bt
                    PRIV_REQUIRES esp32-i2c-lsm6dsox-lis3mdl
                    INCLUDE_DIRS ".")

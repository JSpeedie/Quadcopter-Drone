idf_component_register(SRCS "esp32-i2c-lsm6dsox.cpp"
                            "esp32-i2c-lis3mdl.cpp"
                       REQUIRES driver
                       INCLUDE_DIRS ".")

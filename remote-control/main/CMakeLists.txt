set(srcs "remote-control.cpp")

idf_component_register(SRCS "${srcs}"
                    REQUIRES driver
                    REQUIRES bt
                    INCLUDE_DIRS ".")

#ifndef LIBREALSENSE_RSCORE_HPP
#define LIBREALSENSE_RSCORE_HPP

//#include "rs.h"
#include <cstddef>

struct rs_device
{
    virtual const char *                    get_name() const = 0;
    virtual const char *                    get_serial() const = 0;
    virtual const char *                    get_firmware_version() const = 0;
};

struct rs_context
{
    virtual size_t                          get_device_count() const = 0;
    virtual rs_device *                     get_device(int index) const = 0;
};

#endif


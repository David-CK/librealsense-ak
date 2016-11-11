#ifndef LIBREALSENSE_R200_H
#define LIBREALSENSE_R200_H

#include "ds-device.h"

namespace rsimpl
{
    std::shared_ptr<rs_device> make_r200_device(std::shared_ptr<uvc::device> device);
}

#endif


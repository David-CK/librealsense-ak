// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_R200_H
#define LIBREALSENSE_R200_H

#include "ds-device.h"

//using namespace rsimpl;

namespace rsimpl
{
    class r200_camera final : public ds::ds_device
    {

    public:
        r200_camera(std::shared_ptr<uvc::device> device, const static_device_info & info);
    };
    std::shared_ptr<rs_device> make_r200_device(std::shared_ptr<uvc::device> device);
}

#endif

// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_ds_device_H
#define LIBREALSENSE_ds_device_H

#include "device.h"
#include "ds-private.h"

#define R200_PRODUCT_ID  0x0a80

namespace rsimpl
{
    namespace ds
    {
        class ds_device: public rs_device_base
        {
        protected:
            time_pad start_stop_pad; // R200 line-up needs minimum 500ms delay between consecutive start-stop commands

        public:
            ds_device(std::shared_ptr<uvc::device> device, const static_device_info & info, calibration_validator validator);
            static void set_common_ds_config(std::shared_ptr<uvc::device> device, static_device_info& info, const ds::ds_info& cam_info);
        };
    }
}

#endif // ds_device_H

#ifndef LIBREALSENSE_UVC_H
#define LIBREALSENSE_UVC_H

#include <memory>
//#include <functional>
#include "types.h"
const uint16_t VID_INTEL_CAMERA     = 0x8086;
const uint16_t ZR300_CX3_PID        = 0x0acb;
const uint16_t ZR300_FISHEYE_PID    = 0x0ad0;

namespace rsimpl
{
    namespace uvc
    {
        struct context; // Opaque type representing access to the underlying UVC implementation
        struct device;  // Opaque type representing access to a specific UVC device

        // Enumerate devices
        std::shared_ptr<context> create_context();
        std::vector<std::shared_ptr<device>> query_devices(std::shared_ptr<context> context);

        // Static device properties
        int get_vendor_id(const device & device);
        int get_product_id(const device & device);
    }
}

#endif


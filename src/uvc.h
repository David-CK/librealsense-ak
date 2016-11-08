#include "types.h"
#include <memory>
//inc
#include <functional>
#include <memory>
//inc

const uint16_t VID_INTEL_CAMERA     = 0x8086;
const uint16_t ZR300_FISHEYE_PID    = 0x0ad0;

namespace rsimpl
{
    namespace uvc
    {
        //struct context;
        //std::shared_ptr<context> create_context();
        struct device;
        std::vector<std::shared_ptr<device>> query_devices();
        int get_vendor_id(const device & device);
        int get_product_id(const device & device);
    }
}


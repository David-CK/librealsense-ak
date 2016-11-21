#ifndef LIBREALSENSE_UVC_H
#define LIBREALSENSE_UVC_H

#include "types.h"

#include <memory>
//#include <functional>
#include <thread>       // For this_thread::sleep_for

const uint16_t VID_INTEL_CAMERA     = 0x8086;
const uint16_t ZR300_CX3_PID        = 0x0acb;
const uint16_t ZR300_FISHEYE_PID    = 0x0ad0;

namespace rsimpl
{
    namespace uvc
    {
        struct guid { uint32_t data1; uint16_t data2, data3; uint8_t data4[8]; };
        struct extension_unit { int subdevice, unit, node; guid id; };

        struct context; // Opaque type representing access to the underlying UVC implementation
        struct device;  // Opaque type representing access to a specific UVC device

        // Enumerate devices
        std::shared_ptr<context> create_context();
        std::vector<std::shared_ptr<device>> query_devices(std::shared_ptr<context> context);

        // Static device properties
        int get_vendor_id(const device & device);
        int get_product_id(const device & device);

        // Access XU controls
        void set_control(device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len);
        void get_control(const device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len);

        inline void set_control_with_retry(device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len)
        {
            // Try writing a control, if it fails, retry several times
            for(int i=0; i<20; ++i)
            {
                try { set_control(device, xu, ctrl, data, len); return; }
                catch(...) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            }
            set_control(device, xu, ctrl, data, len);
        }
        
        inline void get_control_with_retry(const device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len)
        {
            // Try reading a control, if it fails, retry several times
            for(int i=0; i<20; ++i)
            {
                try { get_control(device, xu, ctrl, data, len); return; }
                catch(...) { std::this_thread::sleep_for(std::chrono::milliseconds(50)); }
            }
            get_control(device, xu, ctrl, data, len);
        }
    }
}

#endif


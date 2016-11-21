#include "image.h"
#include "ds-device.h"

using namespace rsimpl::ds;

namespace rsimpl
{
    void ds_device::set_common_ds_config(std::shared_ptr<uvc::device> device, static_device_info& info, const ds::ds_info& cam_info)
    {
        auto & c = cam_info.calibration;
        info.capabilities_vector.push_back(RS_CAPABILITIES_ENUMERATION);
        info.capabilities_vector.push_back(RS_CAPABILITIES_COLOR);
        info.capabilities_vector.push_back(RS_CAPABILITIES_DEPTH);
        info.capabilities_vector.push_back(RS_CAPABILITIES_INFRARED);
        info.capabilities_vector.push_back(RS_CAPABILITIES_INFRARED2);

        info.stream_subdevices[RS_STREAM_DEPTH] = 1;
        info.stream_subdevices[RS_STREAM_COLOR] = 2;
        info.stream_subdevices[RS_STREAM_INFRARED] = 0;
        info.stream_subdevices[RS_STREAM_INFRARED2] = 0;

        for(auto fps : {30, 60, 90})
        {
            for(auto pf : {pf_y8, pf_y8i, pf_y16, pf_y12i})
            {
             ;//
            }
        }
    }
}


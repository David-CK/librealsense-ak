#include "r200.h"

//using namespace rsimpl;
//using namespace rsimpl::ds;

namespace rsimpl
{
    std::shared_ptr<rs_device> make_r200_device(std::shared_ptr<uvc::device> device)
    {
        LOG_INFO("Connecting to Intel RealSense R200");

       static_device_info info;
       info.name = "Intel RealSense R200";
       auto cam_info = ds::read_camera_info(*device); //CKCKCK
    }
}


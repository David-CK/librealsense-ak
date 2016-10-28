#include "../include/librealsense/rscore.hpp"
#include <vector>
#include <memory>

namespace rsimpl
{
    struct static_device_info
    {
        std::string name;                                                   // Model name of the camera
    };

    struct device_config
    {
        const static_device_info            info;
    };
}


#ifndef LIBREALSENSE_TYPES_H
#define LIBREALSENSE_TYPES_H

#include "../include/librealsense/rs.h"
#include "../include/librealsense/rscore.hpp"
#include <vector>
#include <sstream>
//#include <memory>
namespace rsimpl
{
    void log(rs_log_severity severity, const std::string & message);
    rs_log_severity get_minimum_severity();
#define LOG(SEVERITY, ...) do { if(static_cast<int>(SEVERITY) >= rsimpl::get_minimum_severity()) { std::ostringstream ss; ss << __VA_ARGS__; rsimpl::log(SEVERITY, ss.str()); } } while(false);
#define LOG_INFO(...)    LOG(RS_LOG_SEVERITY_INFO, __VA_ARGS__)
/*
    struct static_device_info
    {
        std::string name;                                                   // Model name of the camera
    };

    struct device_config
    {
        const static_device_info            info;
    };
*/
}
#endif

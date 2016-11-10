#ifndef LIBREALSENSE_TYPES_H
#define LIBREALSENSE_TYPES_H

#include "../include/librealsense/rs.h"
#include "../include/librealsense/rscore.hpp"
#include <vector>
#include <sstream>
//#include <memory>
//inc
#include <cassert>
#include <cstring>
#include <vector>
#include <sstream>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <atomic>
#include <map>
#include <algorithm>
//inc
namespace rsimpl
{
    ///////////////////////
    // Logging mechanism //
    ///////////////////////

    void log(rs_log_severity severity, const std::string & message);
    void log_to_console(rs_log_severity min_serverity);
    void log_to_file(rs_log_severity min_serverity, const char * file_path);
    rs_log_severity get_minimum_severity();

#define LOG(SEVERITY, ...) do { printf("do... = %d\n", rsimpl::get_minimum_severity()); if(static_cast<int>(SEVERITY) >= rsimpl::get_minimum_severity()) { std::ostringstream ss; printf("log begin\n"); ss << __VA_ARGS__; rsimpl::log(SEVERITY, ss.str()); }printf("while...\n"); } while(false);
#define LOG_INFO(...)    {printf("log-info\n");LOG(RS_LOG_SEVERITY_INFO, __VA_ARGS__)}
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

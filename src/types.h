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
    struct to_string
    {
        std::ostringstream ss;
        template<class T> to_string & operator << (const T & val) {ss << val; return *this; }
        operator std::string() const {return ss.str(); }
    };

    ///////////////////////
    // Logging mechanism //
    ///////////////////////

    void log(rs_log_severity severity, const std::string & message);
    void log_to_console(rs_log_severity min_serverity);
    void log_to_file(rs_log_severity min_serverity, const char * file_path);
    rs_log_severity get_minimum_severity();

#define LOG(SEVERITY, ...) do { if(static_cast<int>(SEVERITY) >= rsimpl::get_minimum_severity()) { std::ostringstream ss; ss << __VA_ARGS__; rsimpl::log(SEVERITY, ss.str()); } } while(false)
#define LOG_DEBUG(...)   LOG(RS_LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    LOG(RS_LOG_SEVERITY_INFO,  __VA_ARGS__)
#define LOG_WARNING(...) LOG(RS_LOG_SEVERITY_WARN,  __VA_ARGS__)
#define LOG_ERROR(...)   LOG(RS_LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   LOG(RS_LOG_SEVERITY_FATAL, __VA_ARGS__)
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

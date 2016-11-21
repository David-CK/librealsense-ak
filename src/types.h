#ifndef LIBREALSENSE_TYPES_H
#define LIBREALSENSE_TYPES_H

#include "../include/librealsense/rs.h"
#include "../include/librealsense/rscore.hpp"
#include <cstring>
#include <vector>
#include <sstream>
#include <condition_variable>
//#include <mutex>
//#include <memory>
//#include <cstring>
//inc
//#include <cassert>
//#include <atomic>
//#include <map>
//#include <algorithm>
//inc
const uint8_t RS_STREAM_NATIVE_COUNT    = 5;

namespace rsimpl
{
    typedef uint8_t byte;

    struct to_string
    {
        std::ostringstream ss;
        template<class T> to_string & operator << (const T & val) {ss << val; return *this; }
        operator std::string() const {return ss.str(); }
    };

    template<class T> class big_endian
    {
        T be_value;
    public:
        operator T () const
        {
            T le_value = 0;
            for (unsigned int i = 0; i < sizeof(T); ++i) reinterpret_cast<char *>(&le_value)[i] = reinterpret_cast<const char *>(&be_value)[sizeof(T) -i - 1];
            return le_value;
        }
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
    struct int2 {int x,y; };

    struct pixel_format_unpacker
    {
        bool requires_processing;
        void(*unpack)(byte * const dest[], const byte * soucrce, int count);
        std::vector<std::pair<rs_stream, rs_format>> outputs;

        //bool
        //rs_
    };

    struct native_pixel_format
    {
        uint32_t fourcc;
        int plane_count;
        size_t bytes_per_pixel;
        std::vector<pixel_format_unpacker> unpackers;

    //    size_t get_image_size(int width, int height) const { return width * height * plane_count * bytes_per_pixel; }

    };

    struct subdevice_mode
    {
        int subdevice;                          // 0, 1, 2, etc...
        int2 native_dims;                       // Resolution advertised over UVC
        native_pixel_format pf;                 // Pixel format advertised over UVC
        int fps;                                // Framerate advertised over UVC
        rs_intrinsics native_intrinsics;        // Intrinsics struct corresponding to the content of image (Note: width,height may be subset of native_dims)
        std::vector<rs_intrinsics> rect_modes;  // Potential intrinsics of image after being rectified in software by librealsense
        std::vector<int> pad_crop_options;      // Acceptable padding/cropping values
    };
    class firmware_version
    {
/*
        int m_major, m_minor, m_patch, m_build;
        bool is_any;
        std::string string_representation;

        std::string to_string() const;

    public:
        firmware_version() : m_major(0), m_minor(0), m_patch(0), m_build(0), is_any(true), string_representation(to_string()) {}
*/
    };

    struct supported_capability
    {
        rs_capabilities    capability;
        firmware_version   from;
        firmware_version   until;
        rs_camera_info     firmware_type;

        supported_capability(rs_capabilities capability)
            : capability(capability), from(), until(), firmware_type(RS_CAMERA_INFO_CAMERA_FIRMWARE_VERSION) {}
    };

    struct static_device_info
    {
        std::string name;                                                   // Model name of the camera
        int stream_subdevices[RS_STREAM_NATIVE_COUNT];
        std::vector<subdevice_mode> subdevice_modes;                        // A list of available modes each subdevice can be put into
        std::vector<supported_capability> capabilities_vector;
    };

/*
    struct device_config
    {
        const static_device_info            info;
    };
*/
}
#endif

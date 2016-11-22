// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

// This header defines vocabulary types and utility mechanisms used ubiquitously by the
// rest of the library. As clearer module boundaries form, declarations might be moved
// out of this file and into more appropriate locations.
#ifndef LIBREALSENSE_TYPES_H
#define LIBREALSENSE_TYPES_H

#include "../include/librealsense/rs.h"
#include "../include/librealsense/rscore.hpp"
#include <cassert>                          // For assert
#include <cstring>
#include <vector>
#include <sstream>
#include <condition_variable>
//#include <mutex>
//#include <memory>
//#include <cstring>
//inc

#include <atomic>
#include <map>          
//#include <algorithm>
//inc
const uint8_t RS_STREAM_NATIVE_COUNT    = 5;

namespace rsimpl
{
    ///////////////////////////////////
    // Utility types for general use //
    ///////////////////////////////////

    typedef uint8_t byte;

    struct to_string
    {
        std::ostringstream ss;
        template<class T> to_string & operator << (const T & val) { ss << val; return *this; }
        operator std::string() const { return ss.str(); }
    };

#pragma pack(push, 1)
    template<class T> class big_endian
    {
        T be_value;
    public:
        operator T () const
        {
            T le_value = 0;
            for (unsigned int i = 0; i < sizeof(T); ++i) reinterpret_cast<char *>(&le_value)[i] = reinterpret_cast<const char *>(&be_value)[sizeof(T) - i - 1];
            return le_value;
        }
    };
#pragma pack(pop)

    ///////////////////////
    // Logging mechanism //
    ///////////////////////

    void log(rs_log_severity severity, const std::string & message);
    void log_to_console(rs_log_severity min_severity);
    void log_to_file(rs_log_severity min_severity, const char * file_path);
    rs_log_severity get_minimum_severity();

#define LOG(SEVERITY, ...) do { if(static_cast<int>(SEVERITY) >= rsimpl::get_minimum_severity()) { std::ostringstream ss; ss << __VA_ARGS__; rsimpl::log(SEVERITY, ss.str()); } } while(false)
#define LOG_DEBUG(...)   LOG(RS_LOG_SEVERITY_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    LOG(RS_LOG_SEVERITY_INFO,  __VA_ARGS__)
#define LOG_WARNING(...) LOG(RS_LOG_SEVERITY_WARN,  __VA_ARGS__)
#define LOG_ERROR(...)   LOG(RS_LOG_SEVERITY_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   LOG(RS_LOG_SEVERITY_FATAL, __VA_ARGS__)
#define RS_ENUM_HELPERS(TYPE, PREFIX) const char * get_string(TYPE value); \
        inline bool is_valid(TYPE value) { return value >= 0 && value < RS_##PREFIX##_COUNT; } \
        inline std::ostream & operator << (std::ostream & out, TYPE value) { if(is_valid(value)) return out << get_string(value); else return out << (int)value; }
    RS_ENUM_HELPERS(rs_stream, STREAM)
    RS_ENUM_HELPERS(rs_format, FORMAT)
    RS_ENUM_HELPERS(rs_preset, PRESET)
    RS_ENUM_HELPERS(rs_distortion, DISTORTION)
    RS_ENUM_HELPERS(rs_option, OPTION)
    RS_ENUM_HELPERS(rs_capabilities, CAPABILITIES)
    RS_ENUM_HELPERS(rs_camera_info, CAMERA_INFO)
    #undef RS_ENUM_HELPERS
    struct int2 {int x,y; };
    struct float3 { float x,y,z; float & operator [] (int i) { return (&x)[i]; } };
    struct float3x3 { float3 x,y,z; float & operator () (int i, int j) { return (&x)[j][i]; } }; // column-major
    struct pose { float3x3 orientation; float3 position; };
    inline float3 operator + (const float3 & a, const float3 & b) { return {a.x+b.x, a.y+b.y, a.z+b.z}; }
    inline float3 operator * (const float3 & a, float b) { return {a.x*b, a.y*b, a.z*b}; }
    inline float3 operator * (const float3x3 & a, const float3 & b) { return a.x*b.x + a.y*b.y + a.z*b.z; }

    ///////////////////
    // Pixel formats //
    ///////////////////

    struct pixel_format_unpacker
    {
        bool requires_processing;
        void(*unpack)(byte * const dest[], const byte * source, int count);
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

    ////////////////////////
    // Static camera info //
    ////////////////////////

    struct subdevice_mode
    {
        int subdevice;                          // 0, 1, 2, etc...
        int2 native_dims;                       // Resolution advertised over UVC
        native_pixel_format pf;                 // Pixel format advertised over UVC
        int fps;                                // Framerate advertised over UVC
        rs_intrinsics native_intrinsics;        // Intrinsics structure corresponding to the content of image (Note: width,height may be subset of native_dims)
        std::vector<rs_intrinsics> rect_modes;  // Potential intrinsics of image after being rectified in software by librealsense
        std::vector<int> pad_crop_options;      // Acceptable padding/cropping values
    };

    struct stream_request
    {
        bool enabled;
        int width, height;
        rs_format format;
        int fps;
        rs_output_buffer_format output_format;

    };

    struct interstream_rule // Requires a.*field + delta == b.*field OR a.*field + delta2 == b.*field
    {
        rs_stream a, b;
        int stream_request::* field;
        int delta, delta2;
        rs_stream bigger;       // if this equals to a or b, this stream must have field value bigger then the other stream
        bool divides, divides2; // divides = a must divide b; divides2 = b must divide a
        bool same_format;
    };

    struct supported_option
    {
        rs_option option;
        double min, max, step, def;
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
        rs_capabilities     capability;
        firmware_version    from;
        firmware_version    until;
        rs_camera_info      firmware_type;

        
        supported_capability(rs_capabilities capability) 
            : capability(capability), from(), until(), firmware_type(RS_CAMERA_INFO_CAMERA_FIRMWARE_VERSION) {}
    };

    struct static_device_info
    {
        std::string name;                                                   // Model name of the camera
        int stream_subdevices[RS_STREAM_NATIVE_COUNT];                      // Which subdevice is used to support each stream, or -1 if stream is unavailable
        std::vector<subdevice_mode> subdevice_modes;                        // A list of available modes each subdevice can be put into
        std::vector<interstream_rule> interstream_rules;                    // Rules which constrain the set of available modes
        stream_request presets[RS_STREAM_NATIVE_COUNT][RS_PRESET_COUNT];    // Presets available for each stream
        std::vector<supported_option> options;
        pose stream_poses[RS_STREAM_NATIVE_COUNT];                          // Static pose of each camera on the device
        int num_libuvc_transfer_buffers;                                    // Number of transfer buffers to use in LibUVC backend
        std::string firmware_version;                                       // Firmware version string
        std::string serial;                                                 // Serial number of the camera (from USB or from SPI memory)
        float nominal_depth_scale;                                          // Default scale
        std::vector<supported_capability> capabilities_vector;
        std::map<rs_camera_info, std::string> camera_info;

    };

    //////////////////////////////////
    // Runtime device configuration //
    //////////////////////////////////

    struct subdevice_mode_selection
    {
    };
    struct device_config
    {
        const static_device_info            info;
        stream_request                      requests[RS_STREAM_NATIVE_COUNT];                       // Modified by enable/disable_stream calls
        float depth_scale;                                              // Scale of depth values

        explicit device_config(const rsimpl::static_device_info & info) : info(info), depth_scale(info.nominal_depth_scale)
        {
        }
       // subdevice_mode_selection select_mode(const stream_request(&requests)[RS_STREAM_NATIVE_COUNT], int subdevice_index) const;
        std::vector<subdevice_mode_selection> select_modes(const stream_request(&requests)[RS_STREAM_NATIVE_COUNT]) const;
        std::vector<subdevice_mode_selection> select_modes() const { return select_modes(requests); }
    };

    inline rs_intrinsics pad_crop_intrinsics(const rs_intrinsics & i, int pad_crop)
    {
        return{ i.width + pad_crop * 2, i.height + pad_crop * 2, i.ppx + pad_crop, i.ppy + pad_crop, i.fx, i.fy, i.model, {i.coeffs[0], i.coeffs[1], i.coeffs[2], i.coeffs[3], i.coeffs[4]} };
    }

    inline rs_intrinsics scale_intrinsics(const rs_intrinsics & i, int width, int height)
    {
        const float sx = (float)width / i.width, sy = (float)height / i.height;
        return{ width, height, i.ppx*sx, i.ppy*sy, i.fx*sx, i.fy*sy, i.model, {i.coeffs[0], i.coeffs[1], i.coeffs[2], i.coeffs[3], i.coeffs[4]} };
    }
    // this class is a convenience wrapper for intrinsics / extrinsics validation methods
    class calibration_validator 
    {
    public:
        calibration_validator();

     //   bool validate_extrinsics(rs_stream from_stream, rs_stream to_stream) const;
      //  bool validate_intrinsics(rs_stream stream) const;

    private:
        std::function<bool(rs_stream from_stream, rs_stream to_stream)> extrinsic_validator;
        std::function<bool(rs_stream stream)> intrinsic_validator;
    };
}

#endif

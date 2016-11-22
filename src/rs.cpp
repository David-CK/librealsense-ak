// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

//#include <functional>
#include <climits>
#include "context.h"

////////////////////////
// API implementation //
////////////////////////

struct rs_error
{
    std::string message;
    const char * function;
    std::string args;
};

// This facility allows for translation of exceptions to rs_error structs at the API boundary
namespace rsimpl
{
    template<class T> void stream_args(std::ostream & out, const char * names, const T & last) { out << names << ':' << last; }
    template<class T, class... U> void stream_args(std::ostream & out, const char * names, const T & first, const U &... rest)
    {
        while(*names && *names != ',') out << *names++;
        out << ':' << first << ", ";
        while(*names && (*names == ',' || isspace(*names))) ++names;
        stream_args(out, names, rest...);
    }

    static void translate_exception(const char * name, std::string args, rs_error ** error)
    {
        try { throw; }
        catch (const std::exception & e) { if (error) *error = new rs_error {e.what(), name, move(args)}; } // todo - Handle case where THIS code throws
        catch (...) { if (error) *error = new rs_error {"unknown error", name, move(args)}; } // todo - Handle case where THIS code throws
    }
}
#define HANDLE_EXCEPTIONS_AND_RETURN(R, ...) catch(...) { std::ostringstream ss; rsimpl::stream_args(ss, #__VA_ARGS__, __VA_ARGS__); rsimpl::translate_exception(__FUNCTION__, ss.str(), error); return R; }
#define VALIDATE_NOT_NULL(ARG) if(!(ARG)) throw std::runtime_error("null pointer passed for argument \"" #ARG "\"");
#define VALIDATE_ENUM(ARG) if(!rsimpl::is_valid(ARG)) { std::ostringstream ss; ss << "bad enum value for argument \"" #ARG "\""; throw std::runtime_error(ss.str()); }
#define VALIDATE_RANGE(ARG, MIN, MAX) if((ARG) < (MIN) || (ARG) > (MAX)) { std::ostringstream ss; ss << "out of range value for argument \"" #ARG "\""; throw std::runtime_error(ss.str()); }
#define VALIDATE_NATIVE_STREAM(ARG) VALIDATE_ENUM(ARG); if(ARG >= RS_STREAM_NATIVE_COUNT) { std::ostringstream ss; ss << "argument \"" #ARG "\" must be a native stream"; throw std::runtime_error(ss.str()); }

int major(int version)
{
    return version / 10000;
}
int minor(int version)
{
    return (version % 10000) / 100;
}
int patch(int version)
{
    return (version % 100);
}

std::string api_version_to_string(int version)
{
    if (major(version) == 0) return rsimpl::to_string() << version;
    return rsimpl::to_string() << major(version) << "." << minor(version) << "." << patch(version);
}

void report_version_mismatch(int runtime, int compiletime)
{
    throw std::runtime_error(rsimpl::to_string() << "API version mismatch: librealsense.so was compiled with API version " 
        << api_version_to_string(runtime) << " but the application was compiled with " 
        << api_version_to_string(compiletime) << "! Make sure correct version of the library is installed (make install)");
}

rs_context * rs_create_context(int api_version, rs_error ** error) try
{
    int runtime_api_version = rs_get_api_version(error);
    if (*error) throw std::runtime_error(rs_get_error_message(*error));

    if ((runtime_api_version < 10) || (api_version < 10))
    {
        // when dealing with version < 1.0.0 that were still using single number for API version, require exact match
        if (api_version != runtime_api_version) 
            report_version_mismatch(runtime_api_version, api_version);
    }
    else if ((major(runtime_api_version) == 1 && minor(runtime_api_version) <= 9) 
          || (major(api_version) == 1 && minor(api_version) <= 9))
    {
        // when dealing with version < 1.10.0, API breaking changes are still possible without minor version change, require exact match
        if (api_version != runtime_api_version) 
            report_version_mismatch(runtime_api_version, api_version);
    }
    else
    {
        // starting with 1.10.0, versions with same patch are compatible
        if ((major(api_version) != major(runtime_api_version)) 
         || (minor(api_version) != minor(runtime_api_version))) 
            report_version_mismatch(runtime_api_version, api_version);
    }

    return rs_context_base::acquire_instance();
}
HANDLE_EXCEPTIONS_AND_RETURN(nullptr, api_version)


int rs_get_device_count(const rs_context * context, rs_error ** error) try
{
    VALIDATE_NOT_NULL(context);
    return (int)context->get_device_count();
}
HANDLE_EXCEPTIONS_AND_RETURN(0, context)

rs_device * rs_get_device(rs_context * context, int index, rs_error ** error) try
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_RANGE(index, 0, (int)context->get_device_count()-1);
    return context->get_device(index);
}
HANDLE_EXCEPTIONS_AND_RETURN(nullptr, context, index)

const char * rs_get_device_name(const rs_device * device, rs_error ** error) try
{
    VALIDATE_NOT_NULL(device);
    return device->get_name();
}
HANDLE_EXCEPTIONS_AND_RETURN(nullptr, device)

const char * rs_get_device_serial(const rs_device * device, rs_error ** error) try
{
    VALIDATE_NOT_NULL(device);
    return device->get_serial();
}
HANDLE_EXCEPTIONS_AND_RETURN(nullptr, device)
const char * rs_get_device_firmware_version(const rs_device * device, rs_error ** error) try
{
    VALIDATE_NOT_NULL(device);
    return device->get_firmware_version();
}
HANDLE_EXCEPTIONS_AND_RETURN(nullptr, device)


#if 0
#endif
void rs_enable_stream(rs_device * device, rs_stream stream, int width, int height, rs_format format, int framerate, rs_error ** error) try
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NATIVE_STREAM(stream);
    VALIDATE_RANGE(width, 0, INT_MAX);
    VALIDATE_RANGE(height, 0, INT_MAX);
    VALIDATE_ENUM(format);
    VALIDATE_RANGE(framerate, 0, INT_MAX);
    device->enable_stream(stream, width, height, format, framerate, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS);
}
HANDLE_EXCEPTIONS_AND_RETURN(, device, stream, width, height, format, framerate)
void rs_start_device(rs_device * device, rs_error ** error) try
{
    VALIDATE_NOT_NULL(device); 
    device->start(rs_source::RS_SOURCE_VIDEO);
}
HANDLE_EXCEPTIONS_AND_RETURN(, device)
// Verify  and provide API version encoded as integer value
int rs_get_api_version(rs_error ** error) try
{
    // Each component type is within [0-99] range
    VALIDATE_RANGE(RS_API_MAJOR_VERSION, 0, 99);
    VALIDATE_RANGE(RS_API_MINOR_VERSION, 0, 99);
    VALIDATE_RANGE(RS_API_PATCH_VERSION, 0, 99);
    return RS_API_VERSION;
}
HANDLE_EXCEPTIONS_AND_RETURN(0, RS_API_MAJOR_VERSION, RS_API_MINOR_VERSION, RS_API_PATCH_VERSION)

const char * rs_get_failed_function(const rs_error * error) { return error ? error->function : nullptr; }
const char * rs_get_failed_args(const rs_error * error) { return error ? error->args.c_str() : nullptr; }
const char * rs_get_error_message(const rs_error * error) { return error ? error->message.c_str() : nullptr; }

void rs_log_to_console(rs_log_severity min_severity, rs_error ** error) try
{
    rsimpl::log_to_console(min_severity);
}
HANDLE_EXCEPTIONS_AND_RETURN(, min_severity)

void rs_log_to_file(rs_log_severity min_severity, const char * file_path, rs_error ** error) try
{
    rsimpl::log_to_file(min_severity, file_path);
}
HANDLE_EXCEPTIONS_AND_RETURN(, min_severity, file_path)

#include <functional>
#include <sstream>
#include <stdio.h>
#include "librealsense/rs.h"

struct rs_error
{
    std::string message;
    const char * function;
    std::string args;
};

const char * rs_get_failed_function(const rs_error * error) { return error ? error->function : nullptr; }
const char * rs_get_failed_args(const rs_error * error) { return error ? error->args.c_str() : nullptr; }
const char * rs_get_error_message(const rs_error * error) { return error ? error->message.c_str() : nullptr; }

#define HANDLE_EXCEPTIONS_AND_RETURN(R, ...) catch(...) { std::ostringstream ss; return R; }
#define VALIDATE_RANGE(ARG, MIN, MAX) if((ARG) < (MIN) || (ARG) > (MAX)) { std::ostringstream ss; ss << "out of range value for argument \"" #ARG "\""; throw std::runtime_error(ss.str()); }

void rs_create_context(int api_version, rs_error ** error)
{
    int runtime_api_version = rs_get_api_version();
    printf("api version = %d\n", runtime_api_version);
}

// Verify  and provide API version encoded as integer value
int rs_get_api_version()
{
    // Each component type is within [0-99] range
    VALIDATE_RANGE(RS_API_MAJOR_VERSION, 0, 99);
    VALIDATE_RANGE(RS_API_MINOR_VERSION, 0, 99);
    VALIDATE_RANGE(RS_API_PATCH_VERSION, 0, 99);
    return RS_API_VERSION;
}
int rs_get_device_count(rs_error ** error)
{
    return 3;
}
void rs_get_device(rs_error ** error)
{

}
const char * rs_get_device_name(rs_error ** error)
{
}
const char * rs_get_device_serial(rs_error ** error)
{
}
const char * rs_get_device_firmware_version(rs_error ** error)
{
}

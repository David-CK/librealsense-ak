#include <functional>
//#include <stdio.h>
#include "context.h"

struct rs_error
{
    std::string message;
    const char * function;
    std::string args;
};


//#define HANDLE_EXCEPTIONS_AND_RETURN(R, ...) catch(...) { std::ostringstream ss; return R; }
#define VALIDATE_NOT_NULL(ARG) if(!(ARG)) throw std::runtime_error("null pointer passed for argument \"" #ARG "\"");
#define VALIDATE_RANGE(ARG, MIN, MAX) if((ARG) < (MIN) || (ARG) > (MAX)) { std::ostringstream ss; ss << "out of range value for argument \"" #ARG "\""; throw std::runtime_error(ss.str()); }

rs_context * rs_create_context(int api_version, rs_error ** error)
{
    int runtime_api_version = rs_get_api_version();
    return rs_context_base::acquire_instance();
}

int rs_get_device_count(const rs_context * context, rs_error ** error)
{
    VALIDATE_NOT_NULL(context);
    return (int)context->get_device_count();
}
rs_device * rs_get_device(rs_context * context, int index, rs_error ** error)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_RANGE(index, 0, (int)context->get_device_count()-1);
    return context->get_device(index);
}
const char * rs_get_device_name(const rs_device * device, rs_error ** error)
{
    VALIDATE_NOT_NULL(device);
    return device->get_name();
}
const char * rs_get_device_serial(const rs_device * device, rs_error ** error)
{
    VALIDATE_NOT_NULL(device);
    return device->get_serial();
}
const char * rs_get_device_firmware_version(const rs_device * device, rs_error ** error)
{
    VALIDATE_NOT_NULL(device);
    return device->get_firmware_version();
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
const char * rs_get_failed_function(const rs_error * error) { return error ? error->function : nullptr; }
const char * rs_get_failed_args(const rs_error * error) { return error ? error->args.c_str() : nullptr; }
const char * rs_get_error_message(const rs_error * error) { return error ? error->message.c_str() : nullptr; }

/* License: Apache 2.0. See LICENSE file in root directory.
Copyright(c) 2015 Intel Corporation. All Rights Reserved. */

/*************************************************\
* librealsense tutorial #1 - Accessing depth data *
\*************************************************/

/* First include the librealsense C header file */
//#include <typeinfo>
#include <librealsense/rs.h>
#include <stdlib.h>
#include <stdio.h>


/* Function calls to librealsense may raise errors of type rs_error */
rs_error * e = 0;
void check_error()
{
    if (e)
    {
        printf("rs_error was raised when calling %s(%s):\n", rs_get_failed_function(e), rs_get_failed_args(e));
        printf("    %s\n", rs_get_error_message(e));
        exit(EXIT_FAILURE);
    }
}

int main()
{
    rs_log_to_console(RS_LOG_SEVERITY_DEBUG, &e);
    check_error();
    rs_log_to_file(RS_LOG_SEVERITY_DEBUG, "librealsense.log", &e);
    check_error();
    /* Create a context object. This object owns the handles to all connected realsense devices. */
    rs_context * ctx = rs_create_context(RS_API_VERSION, &e);
    check_error();
//    printf("There are %d connected RealSense devices.\n", rs_get_device_count(ctx, &e));
    //std::cout<<typeid(ctx).name()<<endl;
#if 0
    check_error();
    if (rs_get_device_count(ctx, &e) == 0) return EXIT_FAILURE;

    /* This tutorial will access only a single device, but it is trivial to extend to multiple devices */
    rs_device * dev = rs_get_device(ctx, 0, &e);
    check_error();
    printf("\nUsing device 0, an %s\n", rs_get_device_name(dev, &e));
    check_error();
    printf("    Serial number: %s\n", rs_get_device_serial(dev, &e));
    check_error();
    printf("    Firmware version: %s\n", rs_get_device_firmware_version(dev, &e));
    check_error();
#endif
    return 0;

}


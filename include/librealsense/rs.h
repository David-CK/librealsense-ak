/* License: Apache 2.0. See LICENSE file in root directory.
   Copyright(c) 2015 Intel Corporation. All Rights Reserved. */

#ifndef LIBREALSENSE_RS_H
#define LIBREALSENSE_RS_H

#ifdef __cplusplus
extern "C" {
#endif

#define RS_API_MAJOR_VERSION    1
#define RS_API_MINOR_VERSION    11
#define RS_API_PATCH_VERSION    0

#define STRINGIFY(arg) #arg
#define VAR_ARG_STRING(arg) STRINGIFY(arg)

/* Version in encoded integer format (1,9,x) -> 1090x note that each component is limited into [0-99] range by design */
#define RS_API_VERSION  (((RS_API_MAJOR_VERSION) * 10000) + ((RS_API_MINOR_VERSION) * 100) + (RS_API_PATCH_VERSION))
/* Return version in "X.Y.Z" formate */
#define RS_API_VERSION_STR (VAR_ARG_STRING(RS_API_MAJOR_VERSION.RS_API_MINOR_VERSION.RS_API_PATCH_VERSION))

typedef struct rs_context rs_context;
typedef struct rs_error rs_error;

const char * rs_get_failed_function  (const rs_error * error);
const char * rs_get_failed_args      (const rs_error * error);
const char * rs_get_error_message    (const rs_error * error);

rs_context * rs_create_context(int api_version, rs_error ** error);
int rs_get_device_count(rs_error ** error);//CK
void rs_get_device(rs_error ** error);//CK
const char * rs_get_device_name(rs_error ** error);
const char * rs_get_device_serial(rs_error ** error);
const char * rs_get_device_firmware_version(rs_error ** error);
int rs_get_api_version();

#ifdef __cplusplus
}
#endif
#endif

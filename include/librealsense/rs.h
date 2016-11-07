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

/* Version in encoded integer format (1,9,x) -> 1090x note that each component is limited into [0-99] range by design*/
#define RS_API_VERSION  (((RS_API_MAJOR_VERSION) * 10000) + ((RS_API_MINOR_VERSION) * 100) + (RS_API_PATCH_VERSION))
/* Return version in "X.Y.Z" format */
#define RS_API_VERSION_STR (VAR_ARG_STRING(RS_API_MAJOR_VERSION.RS_API_MINOR_VERSION.RS_API_PATCH_VERSION))

typedef enum rs_log_severity {
    RS_LOG_SEVERITY_DEBUG, /* Detailed information about ordinary operations */
    RS_LOG_SEVERITY_INFO,  /* Terse information about ordinary operations */
    RS_LOG_SEVERITY_WARN,  /* Indication of possible failure */
    RS_LOG_SEVERITY_ERROR, /* Indication of definite failure */
    RS_LOG_SEVERITY_FATAL, /* Indication of unrecoverable failure */
    RS_LOG_SEVERITY_NONE,  /* No logging will occur */
    RS_LOG_SEVERITY_COUNT
} rs_log_serverity;

typedef struct rs_context rs_context;
typedef struct rs_device rs_device;
typedef struct rs_error rs_error;

rs_context * rs_create_context(int api_version, rs_error ** error);
/**
 * determine number of connected devices
 * \param[out] error  if non-null, receives any error that occurs during this call, otherwise, errors are ignored
 * \return            the count of devices
 */
int rs_get_device_count(const rs_context * context, rs_error ** error);

/**
 * retrieve connected device by index
 * \param[in] index   the zero based index of device to retrieve
 * \param[out] error  if non-null, receives any error that occurs during this call, otherwise, errors are ignored
 * \return            the requested device
 */
rs_device * rs_get_device(rs_context * context, int index, rs_error ** error);

/**
 * retrieve a human readable device model string
 * \param[out] error  if non-null, receives any error that occurs during this call, otherwise, errors are ignored
 * \return            the model string, such as "Intel Realsense F200" or "Intel Realsense R200"
 */
const char * rs_get_device_name(const rs_device * device, rs_error ** error);
/**
 * retrieve the unique serial number of the device
 * \param[out] error  if non-null, receives any error that occurs during this call, otherwise, errors are ignored
 * \return            the serial number, in a format specific to the device model
 */
const char * rs_get_device_serial(const rs_device * device, rs_error ** error);
/**
 * retrieve the version of the firmware currently installed on the device
 * \param[out] error  if non-null, receives any error that occurs during this call, otherwise, errors are ignored
 * \return            firmware version string, in a format is specific to device model
 */
const char * rs_get_device_firmware_version(const rs_device * device, rs_error ** error);
int rs_get_api_version();

const char * rs_get_failed_function  (const rs_error * error);
const char * rs_get_failed_args      (const rs_error * error);
const char * rs_get_error_message    (const rs_error * error);

#ifdef __cplusplus
}
#endif
#endif
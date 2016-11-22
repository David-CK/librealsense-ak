// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_RSCORE_HPP
#define LIBREALSENSE_RSCORE_HPP

//#include "rs.h"
#include <cstddef>

// this interface is an entry point for extending librealsense with custom devices while keeping librealsense public API
// the interfaces in this file are expected to be inherited from, never used directly, rs.hpp provides safer and simpler wrapper
// an example usage would be playback and record. this API allows to integrate custom playback&record implementation while keeping librealsense API

struct rs_stream_interface
{
};
struct rs_device
{
    virtual const char *                    get_name() const = 0;
    virtual const char *                    get_serial() const = 0;
    virtual const char *                    get_firmware_version() const = 0;
    virtual void                            enable_stream(rs_stream stream, int width, int height, rs_format format, int fps, rs_output_buffer_format output) = 0;
    virtual void                            start(rs_source source) = 0;
    virtual bool                            supports(rs_capabilities capability) const = 0;
};

struct rs_context
{
    virtual size_t                          get_device_count() const = 0;
    virtual rs_device *                     get_device(int index) const = 0;
};

#endif

// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_STREAM_H
#define LIBREALSENSE_STREAM_H

#include "types.h"

#include <memory> // For shared_ptr

namespace rsimpl
{
    struct stream_interface : rs_stream_interface
    {
        stream_interface(calibration_validator in_validator, rs_stream in_stream) : stream(in_stream), validator(in_validator){};
                                                                 

        const rs_stream   stream;

    protected:
        calibration_validator validator;
    };
    class syncronizing_archive;

    struct native_stream final : public stream_interface
    {
        const device_config &                   config;
        
        std::shared_ptr<syncronizing_archive>   archive;

                                                native_stream(device_config & config, rs_stream stream, calibration_validator in_validator);

    };
    class point_stream final : public stream_interface
    {
        const stream_interface &                source;
        mutable unsigned long long              number;
    public:
        point_stream(const stream_interface & source) :stream_interface(calibration_validator(), RS_STREAM_POINTS), source(source), number() {}
    };

    class rectified_stream final : public stream_interface
    {
        const stream_interface &                source;
        mutable unsigned long long              number;
    public:
        rectified_stream(const stream_interface & source) : stream_interface(calibration_validator(), RS_STREAM_RECTIFIED_COLOR), source(source), number() {}
    };
    class aligned_stream final : public stream_interface
    {
        const stream_interface &                from, & to;
        mutable unsigned long long              number;
    public:
        aligned_stream(const stream_interface & from, const stream_interface & to) :stream_interface(calibration_validator(), RS_STREAM_COLOR_ALIGNED_TO_DEPTH), from(from), to(to), number() {}
    };
}

#endif

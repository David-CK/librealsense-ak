// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "stream.h"
using namespace rsimpl;
native_stream::native_stream(device_config & config, rs_stream stream, calibration_validator in_validator) : stream_interface(in_validator, stream), config(config)
{
}

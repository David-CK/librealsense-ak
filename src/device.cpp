// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "device.h"
using namespace rsimpl;

const int MAX_FRAME_QUEUE_SIZE = 20;
const int MAX_EVENT_QUEUE_SIZE = 500;
const int MAX_EVENT_TINE_OUT   = 10;

rs_device_base::rs_device_base(std::shared_ptr<rsimpl::uvc::device> device, const rsimpl::static_device_info & info, calibration_validator validator) : device(device), config(info),
    depth(config, RS_STREAM_DEPTH, validator), color(config, RS_STREAM_COLOR, validator), infrared(config, RS_STREAM_INFRARED, validator), infrared2(config, RS_STREAM_INFRARED2, validator), fisheye(config, RS_STREAM_FISHEYE, validator),
    points(depth), rect_color(color), color_to_depth(color, depth), depth_to_color(depth, color), depth_to_rect_color(depth, rect_color), infrared2_to_depth(infrared2,depth), depth_to_infrared2(depth,infrared2),
    capturing(false), data_acquisition_active(false), max_publish_list_size(MAX_FRAME_QUEUE_SIZE), event_queue_size(MAX_EVENT_QUEUE_SIZE), events_timeout(MAX_EVENT_TINE_OUT),
    usb_port_id(""), motion_module_ready(false), keep_fw_logger_alive(false), frames_drops_counter(0)
{
    streams[RS_STREAM_DEPTH    ] = native_streams[RS_STREAM_DEPTH]     = &depth;
    streams[RS_STREAM_COLOR    ] = native_streams[RS_STREAM_COLOR]     = &color;
    streams[RS_STREAM_INFRARED ] = native_streams[RS_STREAM_INFRARED]  = &infrared;
    streams[RS_STREAM_INFRARED2] = native_streams[RS_STREAM_INFRARED2] = &infrared2;
    streams[RS_STREAM_FISHEYE  ] = native_streams[RS_STREAM_FISHEYE]   = &fisheye;
    streams[RS_STREAM_POINTS]                                          = &points;
    streams[RS_STREAM_RECTIFIED_COLOR]                                 = &rect_color;
    streams[RS_STREAM_COLOR_ALIGNED_TO_DEPTH]                          = &color_to_depth;
    streams[RS_STREAM_DEPTH_ALIGNED_TO_COLOR]                          = &depth_to_color;
    streams[RS_STREAM_DEPTH_ALIGNED_TO_RECTIFIED_COLOR]                = &depth_to_rect_color;
    streams[RS_STREAM_INFRARED2_ALIGNED_TO_DEPTH]                      = &infrared2_to_depth;
    streams[RS_STREAM_DEPTH_ALIGNED_TO_INFRARED2]                      = &depth_to_infrared2;
}




void rs_device_base::enable_stream(rs_stream stream, int width, int height, rs_format format, int fps, rs_output_buffer_format output)
{
    if(capturing) throw std::runtime_error("streams cannot be reconfigured after having called rs_start_device()");
    if(config.info.stream_subdevices[stream] == -1) throw std::runtime_error("unsupported stream");

    config.requests[stream] = { true, width, height, format, fps, output };
    for(auto & s : native_streams) s->archive.reset(); // Changing stream configuration invalidates the current stream info
}

void rs_device_base::start_motion_tracking()
{
}
void rs_device_base::start(rs_source source)
{
    if (source == RS_SOURCE_MOTION_TRACKING)
    {
        if (supports(RS_CAPABILITIES_MOTION_EVENTS))
            start_motion_tracking();
        else
            throw std::runtime_error("motion-tracking is not supported by this device");
    }
    else if (source == RS_SOURCE_VIDEO)
    {
        start_video_streaming();
    }
    else if (source == RS_SOURCE_ALL)
    {
        start(RS_SOURCE_MOTION_TRACKING);
        start(RS_SOURCE_VIDEO);
    }
    else
    {
        throw std::runtime_error("unsupported streaming source!");
    }
}

void rs_device_base::start_video_streaming()
{
    if(capturing) throw std::runtime_error("cannot restart device without first stopping device");

    auto capture_start_time = std::chrono::high_resolution_clock::now();
    auto selected_modes = config.select_modes();
}
void rs_device_base::update_device_info(rsimpl::static_device_info& info)
{
    info.options.push_back({ RS_OPTION_FRAMES_QUEUE_SIZE,     1, MAX_FRAME_QUEUE_SIZE,      1, MAX_FRAME_QUEUE_SIZE });
}

bool rs_device_base::supports(rs_capabilities capability) const
{
}




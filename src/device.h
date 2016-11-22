// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_DEVICE_H
#define LIBREALSENSE_DEVICE_H

#include "uvc.h"
#include "stream.h"
#include <memory>

struct rs_device_base : rs_device
{
private:
    const std::shared_ptr<rsimpl::uvc::device>  device;
protected:
    rsimpl::device_config                       config;
private:
    rsimpl::native_stream                       depth, color, infrared, infrared2, fisheye;
    rsimpl::point_stream                        points;
    rsimpl::rectified_stream                    rect_color;
    rsimpl::aligned_stream                      color_to_depth, depth_to_color, depth_to_rect_color, infrared2_to_depth, depth_to_infrared2;
    rsimpl::native_stream *                     native_streams[RS_STREAM_NATIVE_COUNT];
    rsimpl::stream_interface *                  streams[RS_STREAM_COUNT];

    bool                                        capturing;
    bool                                        data_acquisition_active;
    std::atomic<uint32_t>                       max_publish_list_size;
    std::atomic<uint32_t>                       event_queue_size;
    std::atomic<uint32_t>                       events_timeout;

    mutable std::string                         usb_port_id;
    virtual void                                start_video_streaming();
    virtual void                                start_motion_tracking();

    bool                                        motion_module_ready;
    std::atomic<bool>                           keep_fw_logger_alive;
    
    std::atomic<int>                            frames_drops_counter;

public:
    rs_device_base(std::shared_ptr<rsimpl::uvc::device> device, const rsimpl::static_device_info & info, rsimpl::calibration_validator validator = rsimpl::calibration_validator());

    const char *                                get_name() const override { return config.info.name.c_str(); }
    const char *                                get_serial() const override { return config.info.serial.c_str(); }
    const char *                                get_firmware_version() const override { return config.info.firmware_version.c_str(); }
    void                                        enable_stream(rs_stream stream, int width, int height, rs_format format, int fps, rs_output_buffer_format output) override;
    virtual void                                start(rs_source source) override;
    virtual bool                                supports(rs_capabilities capability) const override;
    static void                                 update_device_info(rsimpl::static_device_info& info);
};

#endif

// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include <climits>
#include "image.h"
#include "ds-device.h"

using namespace rsimpl::ds;

// DS4 Exposure ROI uses stream resolution as control constraints
// When stream disabled, we are supposed to use VGA as the default
#define MAX_DS_DEFAULT_X 639
#define MAX_DS_DEFAULT_Y 439

namespace rsimpl
{
    ds_device::ds_device(std::shared_ptr<uvc::device> device, const static_device_info & info, calibration_validator validator)
        : rs_device_base(device, info, validator), start_stop_pad(std::chrono::milliseconds(500))
    {
    }

    void ds_device::set_common_ds_config(std::shared_ptr<uvc::device> device, static_device_info& info, const ds::ds_info& cam_info)
    {
        auto & c = cam_info.calibration;
        info.capabilities_vector.push_back(RS_CAPABILITIES_ENUMERATION);
        info.capabilities_vector.push_back(RS_CAPABILITIES_COLOR);
        info.capabilities_vector.push_back(RS_CAPABILITIES_DEPTH);
        info.capabilities_vector.push_back(RS_CAPABILITIES_INFRARED);
        info.capabilities_vector.push_back(RS_CAPABILITIES_INFRARED2);

        info.stream_subdevices[RS_STREAM_DEPTH] = 1;
        info.stream_subdevices[RS_STREAM_COLOR] = 2;
        info.stream_subdevices[RS_STREAM_INFRARED] = 0;
        info.stream_subdevices[RS_STREAM_INFRARED2] = 0;

        // Set up modes for left/right/z images
        for(auto fps : {30, 60, 90})
        {
            // Subdevice 0 can provide left/right infrared via four pixel formats, in three resolutions, which can either be uncropped or cropped to match Z
            for(auto pf : {pf_y8, pf_y8i, pf_y16, pf_y12i})
            {
                info.subdevice_modes.push_back({0, {640, 481}, pf, fps, c.modesLR[0], {}, {0, -6}});
                info.subdevice_modes.push_back({0, {640, 373}, pf, fps, c.modesLR[1], {}, {0, -6}});
                info.subdevice_modes.push_back({0, {640, 254}, pf, fps, c.modesLR[2], {}, {0, -6}});
            }

            // Subdevice 1 can provide depth, in three resolutions, which can either be unpadded or padded to match left/right
            info.subdevice_modes.push_back({ 1,{ 628, 469 }, pf_z16,  fps, pad_crop_intrinsics(c.modesLR[0], -6),{},{ 0, +6 } });
            info.subdevice_modes.push_back({ 1,{ 628, 361 }, pf_z16,  fps, pad_crop_intrinsics(c.modesLR[1], -6),{},{ 0, +6 } });
            info.subdevice_modes.push_back({ 1,{ 628, 242 }, pf_z16,  fps, pad_crop_intrinsics(c.modesLR[2], -6),{},{ 0, +6 } });
        }

        // Subdevice 2 can provide color, in several formats and framerates        
        info.subdevice_modes.push_back({ 2, { 640, 480 }, pf_yuy2, 60, c.intrinsicsThird[1], { c.modesThird[1][0] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 640, 480 }, pf_yuy2, 30, c.intrinsicsThird[1], { c.modesThird[1][0] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 320, 240 }, pf_yuy2, 60, scale_intrinsics(c.intrinsicsThird[1], 320, 240), { c.modesThird[1][1] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 320, 240 }, pf_yuy2, 30, scale_intrinsics(c.intrinsicsThird[1], 320, 240), { c.modesThird[1][1] }, { 0 } });
        
        info.subdevice_modes.push_back({ 2,{ 1920, 1080 }, pf_yuy2, 15, c.intrinsicsThird[0],{ c.modesThird[0][0] },{ 0 } });
        info.subdevice_modes.push_back({ 2,{ 1920, 1080 }, pf_yuy2, 30, c.intrinsicsThird[0],{ c.modesThird[0][0] },{ 0 } });


        //                             subdev native-dim      pf   fps           native_intrinsics                              rect_modes      crop-options
        //                             ------ ----------      --   ---           -----------------                              ----------      ------------
        info.subdevice_modes.push_back({ 2,{ 1280, 720 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[0], 1280, 720),  { c.modesThird[0][1] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 960, 540 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[0], 960, 540),   { c.modesThird[0][1] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 848, 480 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[0], 848, 480),   { c.modesThird[0][1] }, { 0 } });
        
        info.subdevice_modes.push_back({ 2, { 640, 480 }, pf_yuy2, 15, c.intrinsicsThird[1], { c.modesThird[1][0] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 640, 480 }, pf_rw16, 15, c.intrinsicsThird[1], { c.modesThird[1][0] }, { 0 } });
        
        info.subdevice_modes.push_back({ 2, { 640, 360 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[1], 640, 360), { c.modesThird[1][0] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 424, 240 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[1], 424, 240), { c.modesThird[1][0] }, { 0 } });

        info.subdevice_modes.push_back({ 2, { 320, 240 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[1], 320, 240), { c.modesThird[1][1] }, { 0 } });
        info.subdevice_modes.push_back({ 2, { 320, 180 }, pf_yuy2, 15, scale_intrinsics(c.intrinsicsThird[1], 320, 180), { c.modesThird[1][1] }, { 0 } });

        // Set up interstream rules for left/right/z images
        for(auto ir : {RS_STREAM_INFRARED, RS_STREAM_INFRARED2})
        {
            info.interstream_rules.push_back({ RS_STREAM_DEPTH, ir, &stream_request::width, 0, 12, RS_STREAM_COUNT, false, false, false });
            info.interstream_rules.push_back({ RS_STREAM_DEPTH, ir, &stream_request::height, 0, 12, RS_STREAM_COUNT, false, false, false });
            info.interstream_rules.push_back({ RS_STREAM_DEPTH, ir, &stream_request::fps, 0, 0, RS_STREAM_COUNT, false, false, false });
        }
        info.interstream_rules.push_back({ RS_STREAM_DEPTH, RS_STREAM_COLOR, &stream_request::fps, 0, 0, RS_STREAM_DEPTH, true, false, false });
        info.interstream_rules.push_back({ RS_STREAM_INFRARED, RS_STREAM_INFRARED2, &stream_request::fps, 0, 0, RS_STREAM_COUNT, false, false, false });
        info.interstream_rules.push_back({ RS_STREAM_INFRARED, RS_STREAM_INFRARED2, &stream_request::width, 0, 0, RS_STREAM_COUNT, false, false, false });
        info.interstream_rules.push_back({ RS_STREAM_INFRARED, RS_STREAM_INFRARED2, &stream_request::height, 0, 0, RS_STREAM_COUNT, false, false, false });
        info.interstream_rules.push_back({ RS_STREAM_INFRARED, RS_STREAM_INFRARED2, nullptr, 0, 0, RS_STREAM_COUNT, false, false, true });

        info.presets[RS_STREAM_INFRARED][RS_PRESET_BEST_QUALITY] = {true, 480, 360, RS_FORMAT_Y8,   60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_DEPTH   ][RS_PRESET_BEST_QUALITY] = {true, 480, 360, RS_FORMAT_Z16,  60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_COLOR   ][RS_PRESET_BEST_QUALITY] = {true, 640, 480, RS_FORMAT_RGB8, 60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};

        info.presets[RS_STREAM_INFRARED][RS_PRESET_LARGEST_IMAGE] = {true, 640,   480, RS_FORMAT_Y8,   60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_DEPTH   ][RS_PRESET_LARGEST_IMAGE] = {true, 640,   480, RS_FORMAT_Z16,  60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_COLOR   ][RS_PRESET_LARGEST_IMAGE] = {true, 1920, 1080, RS_FORMAT_RGB8, 30, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};

        info.presets[RS_STREAM_INFRARED][RS_PRESET_HIGHEST_FRAMERATE] = {true, 320, 240, RS_FORMAT_Y8,   60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_DEPTH   ][RS_PRESET_HIGHEST_FRAMERATE] = {true, 320, 240, RS_FORMAT_Z16,  60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};
        info.presets[RS_STREAM_COLOR   ][RS_PRESET_HIGHEST_FRAMERATE] = {true, 640, 480, RS_FORMAT_RGB8, 60, RS_OUTPUT_BUFFER_FORMAT_CONTINUOUS};

        for(int i=0; i<RS_PRESET_COUNT; ++i)
            info.presets[RS_STREAM_INFRARED2][i] = info.presets[RS_STREAM_INFRARED][i];

        // Extended controls ranges cannot be retrieved from device, therefore the data is locally defined
        //Option                                                                        Min     Max     Step    Default
        info.options.push_back({ RS_OPTION_R200_LR_AUTO_EXPOSURE_ENABLED,               0,      1,      1,      0 });
        info.options.push_back({ RS_OPTION_R200_EMITTER_ENABLED,                        0,      1,      1,      0 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_UNITS,                            0, INT_MAX,     1,      1000 });  // What is the real range?
        info.options.push_back({ RS_OPTION_R200_DEPTH_CLAMP_MIN,                        0, USHRT_MAX,   1,      0 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CLAMP_MAX,                        0, USHRT_MAX,   1,      USHRT_MAX });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_MEAN_INTENSITY_SET_POINT, 0,      4095,   1,      512 });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_BRIGHT_RATIO_SET_POINT,   0,      1,      1,      0 });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_KP_GAIN,                  0,      1000,   1,      0 });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_KP_EXPOSURE,              0,      1000,   1,      0 });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_KP_DARK_THRESHOLD,        0,      1000,   1,      0 });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_TOP_EDGE,                 0,      MAX_DS_DEFAULT_Y,    1,      MAX_DS_DEFAULT_Y });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_BOTTOM_EDGE,              0,      MAX_DS_DEFAULT_Y,    1,      MAX_DS_DEFAULT_Y});
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_LEFT_EDGE,                0,      MAX_DS_DEFAULT_X,    1,      MAX_DS_DEFAULT_X });
        info.options.push_back({ RS_OPTION_R200_AUTO_EXPOSURE_RIGHT_EDGE,               0,      MAX_DS_DEFAULT_X,    1,      MAX_DS_DEFAULT_X });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_DECREMENT, 0,     0xFF,   1,      5 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_INCREMENT, 0,     0xFF,   1,      5 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_MEDIAN_THRESHOLD,         0,      0x3FF,  1,      0xc0 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_SCORE_MINIMUM_THRESHOLD,  0,      0x3FF,  1,      1 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_SCORE_MAXIMUM_THRESHOLD,  0,      0x3FF,  1,      0x200 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_COUNT_THRESHOLD,  0,      0x1F,   1,      6 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_TEXTURE_DIFFERENCE_THRESHOLD, 0,  0x3FF,  1,      0x18 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_SECOND_PEAK_THRESHOLD,    0,      0x3FF,  1,      0x1b });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_NEIGHBOR_THRESHOLD,       0,      0x3FF,  1,      0x7 });
        info.options.push_back({ RS_OPTION_R200_DEPTH_CONTROL_LR_THRESHOLD,             0,      0x7FF,  1,      0x18 });

        // We select the depth/left infrared camera's viewpoint to be the origin
        info.stream_poses[RS_STREAM_DEPTH] = {{{1,0,0},{0,1,0},{0,0,1}},{0,0,0}};
        info.stream_poses[RS_STREAM_INFRARED] = {{{1,0,0},{0,1,0},{0,0,1}},{0,0,0}};

        // The right infrared camera is offset along the +x axis by the baseline (B)
        info.stream_poses[RS_STREAM_INFRARED2] = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}, {c.B * 0.001f, 0, 0}}; // Sterling comment

        // The transformation between the depth camera and third camera is described by a translation vector (T), followed by rotation matrix (Rthird)
        for(int i=0; i<3; ++i) for(int j=0; j<3; ++j)
            info.stream_poses[RS_STREAM_COLOR].orientation(i,j) = c.Rthird[i*3+j];
        for(int i=0; i<3; ++i)
            info.stream_poses[RS_STREAM_COLOR].position[i] = c.T[i] * 0.001f;

        // Our position is added AFTER orientation is applied, not before, so we must multiply Rthird * T to compute it
        info.stream_poses[RS_STREAM_COLOR].position = info.stream_poses[RS_STREAM_COLOR].orientation * info.stream_poses[RS_STREAM_COLOR].position;
        info.nominal_depth_scale = 0.001f;
        info.serial = std::to_string(c.serial_number);
        info.firmware_version = ds::read_firmware_version(*device);

        auto &h = cam_info.head_content;
        info.camera_info[RS_CAMERA_INFO_CAMERA_FIRMWARE_VERSION] = info.firmware_version;
        info.camera_info[RS_CAMERA_INFO_DEVICE_SERIAL_NUMBER] = info.serial;
        info.camera_info[RS_CAMERA_INFO_DEVICE_NAME] = info.name;

        info.camera_info[RS_CAMERA_INFO_ISP_FW_VERSION]         = ds::read_isp_firmware_version(*device);
        info.camera_info[RS_CAMERA_INFO_IMAGER_MODEL_NUMBER]    = to_string() << h.imager_model_number;
        info.camera_info[RS_CAMERA_INFO_CAMERA_TYPE]            = to_string() << h.prq_type;
        info.camera_info[RS_CAMERA_INFO_OEM_ID]                 = to_string() << h.oem_id;
        info.camera_info[RS_CAMERA_INFO_MODULE_VERSION]         = to_string() << (int)h.module_version << "." << (int)h.module_major_version << "." << (int)h.module_minor_version << "." << (int)h.module_skew_version;
        info.camera_info[RS_CAMERA_INFO_FOCUS_VALUE]            = to_string() << h.platform_camera_focus;
        info.camera_info[RS_CAMERA_INFO_CONTENT_VERSION]        = to_string() << h.camera_head_contents_version;
        info.camera_info[RS_CAMERA_INFO_LENS_TYPE]              = to_string() << h.lens_type;
        info.camera_info[RS_CAMERA_INFO_LENS_COATING__TYPE]     = to_string() << h.lens_coating_type;
        info.camera_info[RS_CAMERA_INFO_3RD_LENS_TYPE]          = to_string() << h.lens_type_third_imager;
        info.camera_info[RS_CAMERA_INFO_3RD_LENS_COATING_TYPE]  = to_string() << h.lens_coating_type_third_imager;
        info.camera_info[RS_CAMERA_INFO_NOMINAL_BASELINE]       = to_string() << h.nominal_baseline << " mm";
        info.camera_info[RS_CAMERA_INFO_3RD_NOMINAL_BASELINE]   = to_string() << h.nominal_baseline_third_imager << " mm";
        info.camera_info[RS_CAMERA_INFO_EMITTER_TYPE]           = to_string() << h.emitter_type;

        if (std::isnormal(h.calibration_date))
            info.camera_info[RS_CAMERA_INFO_CALIBRATION_DATE] = time_to_string(h.calibration_date);
        if (std::isnormal(h.first_program_date))
            info.camera_info[RS_CAMERA_INFO_PROGRAM_DATE] = time_to_string(h.first_program_date);
        if (std::isnormal(h.focus_alignment_date))
            info.camera_info[RS_CAMERA_INFO_FOCUS_ALIGNMENT_DATE] = time_to_string(h.focus_alignment_date);
        if (std::isnormal(h.build_date))
            info.camera_info[RS_CAMERA_INFO_BUILD_DATE] = time_to_string(h.build_date);

        // On LibUVC backends, the R200 should use four transfer buffers
        info.num_libuvc_transfer_buffers = 4;

        rs_device_base::update_device_info(info);
    }
}


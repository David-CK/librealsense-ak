// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "types.h"

#define unknown "UNKNOWN" 

namespace rsimpl
{
    const char * get_string(rs_stream value)
    {
        #define CASE(X) case RS_STREAM_##X: return #X;
        switch(value)
        {
        CASE(DEPTH)
        CASE(COLOR)
        CASE(INFRARED)
        CASE(INFRARED2)
        CASE(POINTS)
        CASE(RECTIFIED_COLOR)
        CASE(COLOR_ALIGNED_TO_DEPTH)
        CASE(DEPTH_ALIGNED_TO_COLOR)
        CASE(DEPTH_ALIGNED_TO_RECTIFIED_COLOR)
        CASE(INFRARED2_ALIGNED_TO_DEPTH)
        CASE(DEPTH_ALIGNED_TO_INFRARED2)
        CASE(FISHEYE)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }

    const char * get_string(rs_format value)
    {
        #define CASE(X) case RS_FORMAT_##X: return #X;
        switch(value)
        {
        CASE(ANY)
        CASE(Z16)
        CASE(DISPARITY16)
        CASE(XYZ32F)
        CASE(YUYV)
        CASE(RGB8)
        CASE(BGR8)
        CASE(RGBA8)
        CASE(BGRA8)
        CASE(Y8)
        CASE(Y16)
        CASE(RAW10)
        CASE(RAW16)
        CASE(RAW8)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }

    const char * get_string(rs_preset value)
    {
        #define CASE(X) case RS_PRESET_##X: return #X;
        switch(value)
        {
        CASE(BEST_QUALITY)
        CASE(LARGEST_IMAGE)
        CASE(HIGHEST_FRAMERATE)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }

    const char * get_string(rs_distortion value)
    {
        #define CASE(X) case RS_DISTORTION_##X: return #X;
        switch(value)
        {
        CASE(NONE)
        CASE(MODIFIED_BROWN_CONRADY)
        CASE(INVERSE_BROWN_CONRADY)
        CASE(FTHETA)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }

    const char * get_string(rs_option value)
    {
        #define CASE(X) case RS_OPTION_##X: return #X;
        switch(value)
        {
        CASE(COLOR_BACKLIGHT_COMPENSATION)
        CASE(COLOR_BRIGHTNESS)
        CASE(COLOR_CONTRAST)
        CASE(COLOR_EXPOSURE)
        CASE(COLOR_GAIN)
        CASE(COLOR_GAMMA)
        CASE(COLOR_HUE)
        CASE(COLOR_SATURATION)
        CASE(COLOR_SHARPNESS)
        CASE(COLOR_WHITE_BALANCE)
        CASE(COLOR_ENABLE_AUTO_EXPOSURE)
        CASE(COLOR_ENABLE_AUTO_WHITE_BALANCE)
        CASE(F200_LASER_POWER)
        CASE(F200_ACCURACY)
        CASE(F200_MOTION_RANGE)
        CASE(F200_FILTER_OPTION)
        CASE(F200_CONFIDENCE_THRESHOLD)
        CASE(F200_DYNAMIC_FPS)
        CASE(SR300_AUTO_RANGE_ENABLE_MOTION_VERSUS_RANGE) 
        CASE(SR300_AUTO_RANGE_ENABLE_LASER)               
        CASE(SR300_AUTO_RANGE_MIN_MOTION_VERSUS_RANGE)    
        CASE(SR300_AUTO_RANGE_MAX_MOTION_VERSUS_RANGE)    
        CASE(SR300_AUTO_RANGE_START_MOTION_VERSUS_RANGE)  
        CASE(SR300_AUTO_RANGE_MIN_LASER)                  
        CASE(SR300_AUTO_RANGE_MAX_LASER)                  
        CASE(SR300_AUTO_RANGE_START_LASER)                
        CASE(SR300_AUTO_RANGE_UPPER_THRESHOLD) 
        CASE(SR300_AUTO_RANGE_LOWER_THRESHOLD)
        CASE(R200_LR_AUTO_EXPOSURE_ENABLED)
        CASE(R200_LR_GAIN)
        CASE(R200_LR_EXPOSURE)
        CASE(R200_EMITTER_ENABLED)
        CASE(R200_DEPTH_UNITS)
        CASE(R200_DEPTH_CLAMP_MIN)
        CASE(R200_DEPTH_CLAMP_MAX)
        CASE(R200_DISPARITY_MULTIPLIER)
        CASE(R200_DISPARITY_SHIFT)
        CASE(R200_AUTO_EXPOSURE_MEAN_INTENSITY_SET_POINT)
        CASE(R200_AUTO_EXPOSURE_BRIGHT_RATIO_SET_POINT)  
        CASE(R200_AUTO_EXPOSURE_KP_GAIN)                 
        CASE(R200_AUTO_EXPOSURE_KP_EXPOSURE)             
        CASE(R200_AUTO_EXPOSURE_KP_DARK_THRESHOLD)       
        CASE(R200_AUTO_EXPOSURE_TOP_EDGE)       
        CASE(R200_AUTO_EXPOSURE_BOTTOM_EDGE)    
        CASE(R200_AUTO_EXPOSURE_LEFT_EDGE)      
        CASE(R200_AUTO_EXPOSURE_RIGHT_EDGE)     
        CASE(R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_DECREMENT)   
        CASE(R200_DEPTH_CONTROL_ESTIMATE_MEDIAN_INCREMENT)   
        CASE(R200_DEPTH_CONTROL_MEDIAN_THRESHOLD)            
        CASE(R200_DEPTH_CONTROL_SCORE_MINIMUM_THRESHOLD)     
        CASE(R200_DEPTH_CONTROL_SCORE_MAXIMUM_THRESHOLD)     
        CASE(R200_DEPTH_CONTROL_TEXTURE_COUNT_THRESHOLD)     
        CASE(R200_DEPTH_CONTROL_TEXTURE_DIFFERENCE_THRESHOLD)
        CASE(R200_DEPTH_CONTROL_SECOND_PEAK_THRESHOLD)       
        CASE(R200_DEPTH_CONTROL_NEIGHBOR_THRESHOLD)
        CASE(R200_DEPTH_CONTROL_LR_THRESHOLD)
        CASE(FISHEYE_EXPOSURE)
        CASE(FISHEYE_GAIN)
        CASE(FISHEYE_STROBE)
        CASE(FISHEYE_EXTERNAL_TRIGGER)
        CASE(FRAMES_QUEUE_SIZE)
        CASE(TOTAL_FRAME_DROPS)
        CASE(FISHEYE_ENABLE_AUTO_EXPOSURE)
        CASE(FISHEYE_AUTO_EXPOSURE_MODE)
        CASE(FISHEYE_AUTO_EXPOSURE_ANTIFLICKER_RATE)
        CASE(FISHEYE_AUTO_EXPOSURE_PIXEL_SAMPLE_RATE)
        CASE(FISHEYE_AUTO_EXPOSURE_SKIP_FRAMES)
        CASE(HARDWARE_LOGGER_ENABLED)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }
    const char * get_string(rs_capabilities value)
    {
        #define CASE(X) case RS_CAPABILITIES_##X: return #X;
        switch(value)
        {
        CASE(DEPTH)
        CASE(COLOR)
        CASE(INFRARED)
        CASE(INFRARED2)
        CASE(FISH_EYE)
        CASE(MOTION_EVENTS)
        CASE(MOTION_MODULE_FW_UPDATE)
        CASE(ADAPTER_BOARD)
        CASE(ENUMERATION)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }
    const char * get_string(rs_camera_info value)
    {
        #define CASE(X) case RS_CAMERA_INFO_##X: return #X;
        switch(value)
        {
        CASE(DEVICE_NAME)
        CASE(DEVICE_SERIAL_NUMBER)
        CASE(CAMERA_FIRMWARE_VERSION)
        CASE(ADAPTER_BOARD_FIRMWARE_VERSION)
        CASE(MOTION_MODULE_FIRMWARE_VERSION)
        CASE(IMAGER_MODEL_NUMBER)
        CASE(CAMERA_TYPE)
        CASE(OEM_ID)
        CASE(MODULE_VERSION)
        CASE(BUILD_DATE)
        CASE(CALIBRATION_DATE)
        CASE(PROGRAM_DATE)
        CASE(FOCUS_ALIGNMENT_DATE)
        CASE(FOCUS_VALUE)
        CASE(CONTENT_VERSION)
        CASE(ISP_FW_VERSION)
        CASE(LENS_TYPE)
        CASE(LENS_COATING__TYPE)
        CASE(NOMINAL_BASELINE)
        CASE(3RD_LENS_TYPE)
        CASE(3RD_LENS_COATING_TYPE)
        CASE(3RD_NOMINAL_BASELINE)
        CASE(EMITTER_TYPE)
        default: assert(!is_valid(value)); return unknown;
        }
        #undef CASE
    }
  //  subdevice_mode_selection device_config::select_mode(const  stream_request(&requests)[RS_STREAM_NATIVE_COUNT], int subdevice_index) const
  //  {
   // }
    std::vector<subdevice_mode_selection> device_config::select_modes(const stream_request (&reqs)[RS_STREAM_NATIVE_COUNT]) const
    {
        // Make a mutable copy of our array
    }
   // //calibration_validator::calibration_validator(std::function<bool(rs_stream, rs_stream)> extrinsic_validator, std::function<bool(rs_stream)> intrinsic_validator)
   //  //   : extrinsic_validator(extrinsic_validator), intrinsic_validator(intrinsic_validator)
   // //{
   // //}
    calibration_validator::calibration_validator()
        : extrinsic_validator([](rs_stream, rs_stream) { return true; }), intrinsic_validator([](rs_stream) { return true; })
    {
    }
}

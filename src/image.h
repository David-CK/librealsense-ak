// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_IMAGE_H
#define LIBREALSENSE_IMAGE_H

#include "types.h"
namespace rsimpl
{
    extern const native_pixel_format pf_raw8;       // Four 8 bit luminance
    extern const native_pixel_format pf_rw10;       // Four 10 bit luminance values in one 40 bit macropixel
    extern const native_pixel_format pf_rw16;       // 10 bit in 16 bit WORD with 6 bit unused
    extern const native_pixel_format pf_yuy2;       // Y0 U Y1 V ordered chroma subsampled macropixel
    extern const native_pixel_format pf_y8;         // 8 bit (left) IR image
    extern const native_pixel_format pf_y8i;        // 8 bits left IR + 8 bits right IR per pixel
    extern const native_pixel_format pf_y16;        // 16 bit left IR image
    extern const native_pixel_format pf_y12i;       // 12 bits left IR + 12 bits right IR per pixel
    extern const native_pixel_format pf_z16;        // 16 bit Z image
    extern const native_pixel_format pf_invz;       // 16 bit Z image
    extern const native_pixel_format pf_f200_invi;  // 8-bit IR image
    extern const native_pixel_format pf_f200_inzi;  // 16-bit Z + 8 bit IR per pixel
    extern const native_pixel_format pf_sr300_invi; // 16-bit IR image
    extern const native_pixel_format pf_sr300_inzi; // Planar 16-bit IR image followed by 16-bit Z image
}

#endif

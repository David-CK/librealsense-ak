// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "image.h"

namespace rsimpl
{
    //////////////////////////////
    // Naive unpacking routines //
    //////////////////////////////
    
    template<size_t SIZE> void copy_pixels(byte * const dest[], const byte * source, int count)
    {
        memcpy(dest[0], source, SIZE * count);
    }

    void copy_raw10(byte * const dest[], const byte * source, int count)
    {
        memcpy(dest[0], source, (5 * (count/4)));
    }

    template<class SOURCE, class UNPACK> void unpack_pixels(byte * const dest[], int count, const SOURCE * source, UNPACK unpack)
    {
        auto out = reinterpret_cast<decltype(unpack(SOURCE())) *>(dest[0]);
        for(int i=0; i<count; ++i) *out++ = unpack(*source++);
    }

    void unpack_y16_from_y8    (byte * const d[], const byte * s, int n) { unpack_pixels(d, n, reinterpret_cast<const uint8_t  *>(s), [](uint8_t  pixel) -> uint16_t { return pixel | pixel << 8; }); }
    void unpack_y16_from_y16_10(byte * const d[], const byte * s, int n) { unpack_pixels(d, n, reinterpret_cast<const uint16_t *>(s), [](uint16_t pixel) -> uint16_t { return pixel << 6; }); }
    void unpack_y8_from_y16_10 (byte * const d[], const byte * s, int n) { unpack_pixels(d, n, reinterpret_cast<const uint16_t *>(s), [](uint16_t pixel) -> uint8_t  { return pixel >> 2; }); }
    /////////////////////////////
    // YUY2 unpacking routines //
    /////////////////////////////
    
    // This templated function unpacks YUY2 into Y8/Y16/RGB8/RGBA8/BGR8/BGRA8, depending on the compile-time parameter FORMAT.
    // It is expected that all branching outside of the loop control variable will be removed due to constant-folding.
    template<rs_format FORMAT> void unpack_yuy2(byte * const d [], const byte * s, int n)
    {
    // CK
    }
    
    //////////////////////////////////////
    // 2-in-1 format splitting routines //
    //////////////////////////////////////

    template<class SOURCE, class SPLIT_A, class SPLIT_B> void split_frame(byte * const dest[], int count, const SOURCE * source, SPLIT_A split_a, SPLIT_B split_b)
    {
        auto a = reinterpret_cast<decltype(split_a(SOURCE())) *>(dest[0]);
        auto b = reinterpret_cast<decltype(split_b(SOURCE())) *>(dest[1]);
        for(int i=0; i<count; ++i)
        {
            *a++ = split_a(*source);
            *b++ = split_b(*source++);
        }    
    }

    struct y8i_pixel { uint8_t l, r; };
    void unpack_y8_y8_from_y8i(byte * const dest[], const byte * source, int count)
    {
        split_frame(dest, count, reinterpret_cast<const y8i_pixel *>(source),
            [](const y8i_pixel & p) -> uint8_t { return p.l; },
            [](const y8i_pixel & p) -> uint8_t { return p.r; });
    }

    struct y12i_pixel { uint8_t rl : 8, rh : 4, ll : 4, lh : 8; int l() const { return lh << 4 | ll; } int r() const { return rh << 8 | rl; } };
    void unpack_y16_y16_from_y12i_10(byte * const dest[], const byte * source, int count)
    {
        split_frame(dest, count, reinterpret_cast<const y12i_pixel *>(source),
            [](const y12i_pixel & p) -> uint16_t { return p.l() << 6 | p.l() >> 4; },  // We want to convert 10-bit data to 16-bit data
            [](const y12i_pixel & p) -> uint16_t { return p.r() << 6 | p.r() >> 4; }); // Multiply by 64 1/16 to efficiently approximate 65535/1023
    }

    struct f200_inzi_pixel { uint16_t z16; uint8_t y8; };
    void unpack_z16_y8_from_f200_inzi(byte * const dest[], const byte * source, int count)
    {
        split_frame(dest, count, reinterpret_cast<const f200_inzi_pixel *>(source),
            [](const f200_inzi_pixel & p) -> uint16_t { return p.z16; },
            [](const f200_inzi_pixel & p) -> uint8_t { return p.y8; });
    }

    void unpack_z16_y16_from_f200_inzi(byte * const dest[], const byte * source, int count)
    {
        split_frame(dest, count, reinterpret_cast<const f200_inzi_pixel *>(source),
            [](const f200_inzi_pixel & p) -> uint16_t { return p.z16; },
            [](const f200_inzi_pixel & p) -> uint16_t { return p.y8 | p.y8 << 8; });
    }

    void unpack_z16_y8_from_sr300_inzi(byte * const dest[], const byte * source, int count)
    {
        auto in = reinterpret_cast<const uint16_t *>(source);
        auto out_ir = reinterpret_cast<uint8_t *>(dest[1]);            
        for(int i=0; i<count; ++i) *out_ir++ = *in++ >> 2;
        memcpy(dest[0], in, count*2);
    }

    void unpack_z16_y16_from_sr300_inzi (byte * const dest[], const byte * source, int count)
    {
        auto in = reinterpret_cast<const uint16_t *>(source);
        auto out_ir = reinterpret_cast<uint16_t *>(dest[1]);
        for(int i=0; i<count; ++i) *out_ir++ = *in++ << 6;
        memcpy(dest[0], in, count*2);
    }

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wmultichar"
    //////////////////////////
    // Native pixel formats //
    //////////////////////////
    const native_pixel_format pf_raw8       = { 'RAW8', 1, 1,{  { false, &copy_pixels<1>,                   { { RS_STREAM_FISHEYE,  RS_FORMAT_RAW8 } } } } };
    const native_pixel_format pf_rw16       = { 'RW16', 1, 2,{  { false, &copy_pixels<2>,                   { { RS_STREAM_COLOR,    RS_FORMAT_RAW16 } } } } };
    const native_pixel_format pf_rw10       = { 'pRAA', 1, 1,{  { false, &copy_raw10,                       { { RS_STREAM_COLOR,    RS_FORMAT_RAW10 } } } } };
    const native_pixel_format pf_yuy2       = { 'YUY2', 1, 2,{  { true,  &unpack_yuy2<RS_FORMAT_RGB8 >,     { { RS_STREAM_COLOR,    RS_FORMAT_RGB8 } } },
                                                                { false, &copy_pixels<2>,                   { { RS_STREAM_COLOR,    RS_FORMAT_YUYV } } },
                                                                { true,  &unpack_yuy2<RS_FORMAT_RGBA8>,     { { RS_STREAM_COLOR,    RS_FORMAT_RGBA8 } } },
                                                                { true,  &unpack_yuy2<RS_FORMAT_BGR8 >,     { { RS_STREAM_COLOR,    RS_FORMAT_BGR8 } } },
                                                                { true,  &unpack_yuy2<RS_FORMAT_BGRA8>,     { { RS_STREAM_COLOR,    RS_FORMAT_BGRA8 } } } } };
    const native_pixel_format pf_y8         = { 'GREY', 1, 1,{  { false, &copy_pixels<1>,                   { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } } } };
    const native_pixel_format pf_y16        = { 'Y16 ', 1, 2,{  { true,  &unpack_y16_from_y16_10,           { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_y8i        = { 'Y8I ', 1, 2,{  { true,  &unpack_y8_y8_from_y8i,            { { RS_STREAM_INFRARED, RS_FORMAT_Y8 },{ RS_STREAM_INFRARED2, RS_FORMAT_Y8 } } } } };
    const native_pixel_format pf_y12i       = { 'Y12I', 1, 3,{  { true,  &unpack_y16_y16_from_y12i_10,      { { RS_STREAM_INFRARED, RS_FORMAT_Y16 },{ RS_STREAM_INFRARED2, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_z16        = { 'Z16 ', 1, 2,{  { false, &copy_pixels<2>,                   { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 } } },
                                                                { false, &copy_pixels<2>,                   { { RS_STREAM_DEPTH,    RS_FORMAT_DISPARITY16 } } } } };
    const native_pixel_format pf_invz       = { 'INVZ', 1, 2, { { false, &copy_pixels<2>,                   { { RS_STREAM_DEPTH, RS_FORMAT_Z16 } } } } };
    const native_pixel_format pf_f200_invi  = { 'INVI', 1, 1, { { false, &copy_pixels<1>,                   { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                                { true,  &unpack_y16_from_y8,               { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_f200_inzi  = { 'INZI', 1, 3,{  { true,  &unpack_z16_y8_from_f200_inzi,     { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                                { true,  &unpack_z16_y16_from_f200_inzi,    { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_sr300_invi = { 'INVI', 1, 2,{  { true,  &unpack_y8_from_y16_10,            { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                                { true,  &unpack_y16_from_y16_10,           { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_sr300_inzi = { 'INZI', 2, 2,{  { true,  &unpack_z16_y8_from_sr300_inzi,    { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                                { true,  &unpack_z16_y16_from_sr300_inzi,   { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
//#pragma GCC diagnostic pop
}

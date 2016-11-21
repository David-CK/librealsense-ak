#include "image.h"

namespace rsimpl
{
    template<size_t SIZE> void copy_pixels(byte * const dest[], const byte * source, int count)
    {
        memcpy(dest[0], source, SIZE * count);
    }

    void copy_raw10(byte * const dest[], const byte * source, int count)
    {
        memcpy(dest[0], source, (5 * (count/4)));
    }

    void unpack_y16_from_y8    (byte * const d[], const byte * s, int n) {/*ck*/}
    void unpack_y16_from_y16_10(byte * const d[], const byte * s, int n) {/*ck*/}
    void unpack_y8_from_y16_10(byte * const d[], const byte * s, int n) {/*ck*/}
    template<rs_format FORMAT> void unpack_yuy2(byte * const d [], const byte *s, int n)
    {
    // CK
    }
    void unpack_y8_y8_from_y8i(byte * const dest[], const byte * source, int n)
    {
    /*ck*/
    }
    void unpack_y16_y16_from_y12i_10(byte * const dest[], const byte * source, int count)
    {
    /*ck*/
    }
    void unpack_z16_y8_from_f200_inzi(byte * const dest[], const byte * source, int count)
    {
    /*ck*/
    }
    void unpack_z16_y16_from_f200_inzi(byte * const dest[], const byte * source, int count)
    {
    /*ck*/
    }
    void unpack_z16_y8_from_sr300_inzi(byte * const dest[], const byte * source, int count)
    {
    /*ck*/
    }
    void unpack_z16_y16_from_sr300_inzi(byte * const dest[], const byte * source, int count)
    {
    /*ck*/
    }

//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wmultichar"
    //////////////////////////
    // Native pixel formats //
    //////////////////////////
    const native_pixel_format pf_raw8       = { 'RAW8', 1, 1,{ { false, &copy_pixels<1>,                    { { RS_STREAM_FISHEYE,  RS_FORMAT_RAW8 } } } } };
    const native_pixel_format pf_rw16       = { 'RW16', 1, 2,{ { false, &copy_pixels<2>,                    { { RS_STREAM_COLOR,    RS_FORMAT_RAW16 } } } } };
    const native_pixel_format pf_rw10       = { 'pRAA', 1, 1,{ { false, &copy_raw10,                        { { RS_STREAM_COLOR,    RS_FORMAT_RAW10 } } } } };
    const native_pixel_format pf_yuy2       = { 'YUY2', 1, 2,{ { true,  &unpack_yuy2<RS_FORMAT_RGB8 >,      { { RS_STREAM_COLOR,    RS_FORMAT_RGB8 } } },
                                                               { false, &copy_pixels<2>,                    { { RS_STREAM_COLOR,    RS_FORMAT_YUYV } } },
                                                               { true,  &unpack_yuy2<RS_FORMAT_RGBA8>,      { { RS_STREAM_COLOR,    RS_FORMAT_RGBA8 } } },
                                                               { true,  &unpack_yuy2<RS_FORMAT_BGR8>,       { { RS_STREAM_COLOR,    RS_FORMAT_BGR8 } } },
                                                               { true,  &unpack_yuy2<RS_FORMAT_BGRA8>,      { { RS_STREAM_COLOR,    RS_FORMAT_BGRA8 } } } } };
    const native_pixel_format pf_y8         = { 'GREY', 1, 1,{ { false, &copy_pixels<1>,                    { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } } } };
    const native_pixel_format pf_y16        = { 'Y16 ', 1, 2,{ { true,  &unpack_y16_from_y16_10,             { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_y8i        = { 'Y8I ', 1, 2,{ { true,  &unpack_y8_y8_from_y8i,             { { RS_STREAM_INFRARED, RS_FORMAT_Y8 },{ RS_STREAM_INFRARED2, RS_FORMAT_Y8 } } } } };
    const native_pixel_format pf_y12i       = { 'Y12I', 1, 3,{ { true,  &unpack_y16_y16_from_y12i_10,       { { RS_STREAM_INFRARED, RS_FORMAT_Y16 },{ RS_STREAM_INFRARED2, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_z16        = { 'Z16 ', 1, 2,{ { false, &copy_pixels<2>,                    { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 } } },
                                                               { false, &copy_pixels<2>,                    { { RS_STREAM_DEPTH,    RS_FORMAT_DISPARITY16 } } } } };
    const native_pixel_format pf_invz       = { 'INVZ', 1, 2,{ { false, &copy_pixels<2>,                    { { RS_STREAM_DEPTH, RS_FORMAT_Z16 } } } } };
    const native_pixel_format pf_f200_invi  = { 'INVI', 1, 1,{ { false, &copy_pixels<1>,                    { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                               { true,  &unpack_y16_from_y8,                { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_f200_inzi  = { 'INZI', 1, 3,{ { true,  &unpack_z16_y8_from_f200_inzi,      { { RS_STREAM_DEPTH,    RS_FORMAT_Y16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                               { true,  &unpack_z16_y16_from_f200_inzi,     { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_sr300_invi = { 'INVI', 1, 2,{ { true,  &unpack_y8_from_y16_10,             { { RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                               { true,  &unpack_y16_from_y16_10,            { { RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
    const native_pixel_format pf_sr300_inzi = { 'INZI', 2, 2,{ { true,  &unpack_z16_y8_from_sr300_inzi,     { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y8 } } },
                                                               { true,  &unpack_z16_y16_from_sr300_inzi,    { { RS_STREAM_DEPTH,    RS_FORMAT_Z16 },{ RS_STREAM_INFRARED, RS_FORMAT_Y16 } } } } };
//#pragma GCC diagnostic pop
}

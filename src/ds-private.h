// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#ifndef LIBREALSENSE_DS_PRIVATE_H
#define LIBREALSENSE_DS_PRIVATE_H

#include "uvc.h"
#include <cmath>
//#include <algorithm>

namespace rsimpl
{
    namespace ds
    {
        const uvc::extension_unit lr_xu = {0, 2, 1, {0x18682d34, 0xdd2c, 0x4073, {0xad, 0x23, 0x72, 0x14, 0x73, 0x9a, 0x07, 0x4c}}};
        struct ds_calibration
        {
            int version;
            uint32_t serial_number;
            rs_intrinsics modesLR[3];
            rs_intrinsics intrinsicsThird[2];
            rs_intrinsics modesThird[2][2];
            float Rthird[9], T[3], B;
        };

        enum ds_lens_type : uint32_t
        {
            DS_LENS_UNKNOWN = 0// CK__,
        };
        enum ds_lens_coating_type : uint32_t
        {
            DS_LENS_COATING_UNKNOWN = 0//CK__,
        };
        enum ds_emitter_type : uint32_t // CK__
        {
            DS_EMITTER_NONE = 0//CK__
        };
        enum ds_oem_id : uint32_t
        {
            DS_OEM_NONE = 0//CK__
        };
        enum ds_prq_type : uint8_t // CK__
        {
            DS_PRQ_READY = 1//CK__
        };
        inline std::string time_to_string(double val)
        {
            std::string date("Undefined value");

            // rigorous validation is required due to improper handling of NAN by gcc
            if (std::isnormal(val) && std::isfinite(val) && (!std::isnan(val)) )
            {
                auto time = time_t(val);
                std::vector<char> outstr;
                outstr.resize(200);
                strftime(outstr.data(),outstr.size(),"%Y-%m-%d %H:%M:%S",std::gmtime(&time));
                date = to_string()<< outstr.data() << " UTC";
            }
            return date;
        }

#pragma pack(push,1)
        /// The struct is aligned with the data layout in device
        struct ds_head_content
        {
            enum { DS_HEADER_VERSION_NUMBER = 12 };               // required camera header version number for DS devices
            uint32_t        serial_number;
            uint32_t        imager_model_number;
            uint32_t        module_revision_number;
            uint8_t         ck__model_data[64];
            double          build_date;
            double          first_program_date;
            double          focus_alignment_date;
            int32_t         nominal_baseline_third_imager;
            uint8_t         module_version;
            uint8_t         module_major_version;
            uint8_t         module_minor_version;
            uint8_t         module_skew_version;
            ds_lens_type    lens_type_third_imager;
            ds_oem_id       oem_id;
            ds_lens_coating_type lens_coating_type_third_imager;
            uint8_t         ck__platform_camera_support;
            ds_prq_type     prq_type;
            uint8_t         ck__reserved1[2];
            ds_emitter_type emitter_type;
            uint8_t         ck__reserved2[4];
            uint32_t        ck__camera_fpag_version;
            uint32_t        platform_camera_focus;                  // This is the value during calibration
            double          calibration_date;
            uint32_t        ck__calibration_type;
            double          ck__calibration_x_error;
            double          ck__calibration_y_error;
            double          ck__rectification_data_qres[54];
            double          ck__rectification_data_padding[26];
            double          ck__cx_qres;
            double          ck__cy_qres;
            double          ck__cz_qres;
            double          ck__kx_qres;
            double          ck__ky_qres;
            uint32_t        camera_head_contents_version;
            uint32_t        ck__camera_head_contents_size_bytes;
            double          ck__cx_big;
            double          ck__cy_big;
            double          ck__cz_big;
            double          ck__kx_big;
            double          ck__ky_big;
            double          ck__cx_special;
            double          ck__cy_special;
            double          ck__cz_special;
            double          ck__kx_special;
            double          ck__ky_special;
            uint8_t         ck__camera_head_data_little_endian;
            double          ck__rectification_data_big[54];
            double          ck__rectification_data_special[54];
            uint8_t         ck__camera_options_1;
            uint8_t         ck__camera_options_2;
            uint8_t         ck__body_serial_number[20];
            double          ck__dx;
            double          ck__dy;
            double          ck__dz;
            double          ck__theta_x;
            double          ck__theta_y;
            double          ck__theta_z;
            double          ck__registration_data;
            double          ck__registration_rotation[9];
            double          ck__registration_translation[3];
            uint32_t        nominal_baseline;
            ds_lens_type    lens_type;
            ds_lens_coating_type    lens_coating_type;
            int32_t         ck__nominal_baseline_platform[3];
            uint32_t        ck__lens_type_platform;
            uint32_t        ck_imager_type_platform;
            uint32_t        ck_the_last_word;
            uint8_t         ck_reserved3[37];
        };

#pragma pack(pop)

        struct ds_info
        {
            ds_head_content head_content;
            ds_calibration calibration;
        };


        ds_info read_camera_info(uvc::device & device);
        std::string read_firmware_version(uvc::device & device);
        std::string read_isp_firmware_version(uvc::device & device);
        enum class control
        {
            command_response           = 1,
        };
        class time_pad
        {
            std::chrono::high_resolution_clock::duration _duration;
        public:
            time_pad(std::chrono::high_resolution_clock::duration duration) : _duration(duration) {}
        };
    }
}
#endif // DS_PRIVATE_H

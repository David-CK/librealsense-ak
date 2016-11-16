#include "uvc.h"
#include <cmath>
//#include <algorithm>

namespace rsimpl
{
    namespace ds
    {
        struct ds_calibration
        {
            int version;
            uint32_t serial_number;
            rs_intrinsics modesLR[3];
        };

        enum ds_lens_type : uint32_t
        {
            //DS_LENS_UNKNOWN = 0,
        };
        enum ds_lens_coating_type : uint32_t
        {
            //DS_LENS_COATING_UNKNOWN = 0,
        };
        enum ds_oem_id : uint32_t
        {
            //DS_OEM_NONE = 0
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

        struct ds_head_content
        {
            enum { DS_HEADER_VERSION_NUMBER = 12 };               // required camera header version number for DS devices
            uint32_t        serial_number;
            uint32_t        imager_model_number;
            uint32_t        module_revision_number;
            double          build_date;
            int32_t         nominal_baseline_third_imager;
            uint8_t         module_version;
            uint8_t         module_major_version;
            uint8_t         module_minor_version;
            uint8_t         module_skew_version;
            ds_lens_type    lens_type_third_imager;
            ds_oem_id       oem_id;
            ds_lens_coating_type lens_coating_type_third_imager;
            uint32_t        camera_head_contents_version;
            double          calibration_date;
            uint32_t        nominal_baseline;
            ds_lens_type    lens_type;
            ds_lens_coating_type    lens_coating_type;
        };

        struct ds_info
        {
            ds_head_content head_content;
            ds_calibration calibration;
        };
        ds_info read_camera_info(uvc::device & device);
    }
}

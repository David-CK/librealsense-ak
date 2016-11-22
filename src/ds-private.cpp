// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include "ds-private.h"

enum class command : uint32_t // Command/response codes
{
    download_spi_flash = 0x1A,
    get_fwrevision     = 0x21
};

enum class command_modifier : uint32_t { direct = 0x10 }; // Command/response modifiers

#define SPI_FLASH_PAGE_SIZE_IN_BYTES                0x100
#define SPI_FLASH_SECTOR_SIZE_IN_BYTES              0x1000
#define SPI_FLASH_SIZE_IN_SECTORS                   256
#define SPI_FLASH_TOTAL_SIZE_IN_BYTES               (SPI_FLASH_SIZE_IN_SECTORS * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define SPI_FLASH_PAGES_PER_SECTOR                  (SPI_FLASH_SECTOR_SIZE_IN_BYTES / SPI_FLASH_PAGE_SIZE_IN_BYTES)
#define SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE     160
#define NV_NON_FIRMWARE_START                       (SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_ADMIN_DATA_N_ENTRIES                     9
#define NV_CALIBRATION_DATA_ADDRESS_INDEX           0
#define NV_NON_FIRMWARE_ROOT_ADDRESS                NV_NON_FIRMWARE_START
#define CAM_INFO_BLOCK_LEN 2048

namespace rsimpl {
    namespace ds
    {

        void xu_read(const uvc::device & device, uvc::extension_unit xu, control xu_ctrl, void * buffer, uint32_t length)
        {
            uvc::get_control_with_retry(device, xu, static_cast<int>(xu_ctrl), buffer, length);
        }

        void xu_write(uvc::device & device, uvc::extension_unit xu, control xu_ctrl, void * buffer, uint32_t length)
        {
            uvc::set_control_with_retry(device, xu, static_cast<int>(xu_ctrl), buffer, length);
        }

        struct CommandResponsePacket
        {
            command code; command_modifier modifier;
            uint32_t tag, address, value, reserved[59];
            CommandResponsePacket() { std::memset(this, 0, sizeof(CommandResponsePacket)); }
            CommandResponsePacket(command code, uint32_t address = 0, uint32_t value = 0) : code(code), modifier(command_modifier::direct), tag(12), address(address), value(value)
            {
                std::memset(reserved, 0, sizeof(reserved));
            }
        };

        CommandResponsePacket send_command_and_receive_response(uvc::device & device, const CommandResponsePacket & command)
        {
            CommandResponsePacket c = command, r;
            set_control(device, lr_xu, static_cast<int>(control::command_response), &c, sizeof(c));
            get_control(device, lr_xu, static_cast<int>(control::command_response), &r, sizeof(r));
            return r;
        }

        bool read_device_pages(uvc::device & dev, uint32_t address, unsigned char * buffer, uint32_t nPages)
        {
            int addressTest = SPI_FLASH_TOTAL_SIZE_IN_BYTES - address - nPages * SPI_FLASH_PAGE_SIZE_IN_BYTES;

            if (!nPages || addressTest < 0)
                return false;

            // This command allows the host to read a block of data from the SPI flash.
            // Once this command is processed by the DS4, further command messages will be treated as SPI data
            // and therefore will be read from flash. The size of the SPI data must be a multiple of 256 bytes.
            // This will repeat until the number of bytes specified in the ‘value’ field of the original command
            // message has been read.  At that point the DS4 will process command messages as expected.

            send_command_and_receive_response(dev, CommandResponsePacket(command::download_spi_flash, address, nPages * SPI_FLASH_PAGE_SIZE_IN_BYTES));

            uint8_t *p = buffer;
            uint16_t spiLength = SPI_FLASH_PAGE_SIZE_IN_BYTES;
            for (unsigned int i = 0; i < nPages; ++i)
            {
                xu_read(dev, lr_xu, control::command_response, p, spiLength);
                p += SPI_FLASH_PAGE_SIZE_IN_BYTES;
            }
            return true;
        }

        void read_arbitrary_chunk(uvc::device & dev, uint32_t address, void * dataIn, int lengthInBytesIn)
        {
            unsigned char * data = (unsigned char *)dataIn;
            int lengthInBytes = lengthInBytesIn;
            unsigned char page[SPI_FLASH_PAGE_SIZE_IN_BYTES];
            int nPagesToRead;
            uint32_t startAddress = address;
            if (startAddress & 0xff)
            {
                // we are not on a page boundary
                startAddress = startAddress & ~0xff;
                uint32_t startInPage = address - startAddress;
                uint32_t lengthToCopy = SPI_FLASH_PAGE_SIZE_IN_BYTES - startInPage;
                if (lengthToCopy > (uint32_t)lengthInBytes)
                    lengthToCopy = lengthInBytes;
                read_device_pages(dev, startAddress, page, 1);
                memcpy(data, page + startInPage, lengthToCopy);
                lengthInBytes -= lengthToCopy;
                data += lengthToCopy;
                startAddress += SPI_FLASH_PAGE_SIZE_IN_BYTES;
            }

            nPagesToRead = lengthInBytes / SPI_FLASH_PAGE_SIZE_IN_BYTES;

            if (nPagesToRead > 0)
                read_device_pages(dev, startAddress, data, nPagesToRead);

            lengthInBytes -= (nPagesToRead * SPI_FLASH_PAGE_SIZE_IN_BYTES);

            if (lengthInBytes)
            {
                // means we still have a remainder
                data += (nPagesToRead * SPI_FLASH_PAGE_SIZE_IN_BYTES);
                startAddress += (nPagesToRead * SPI_FLASH_PAGE_SIZE_IN_BYTES);
                read_device_pages(dev, startAddress, page, 1);
                memcpy(data, page, lengthInBytes);
            }
        }

        bool read_admin_sector(uvc::device & dev, unsigned char data[SPI_FLASH_SECTOR_SIZE_IN_BYTES], int whichAdminSector)
        {
            uint32_t adminSectorAddresses[NV_ADMIN_DATA_N_ENTRIES];

            read_arbitrary_chunk(dev, NV_NON_FIRMWARE_ROOT_ADDRESS, adminSectorAddresses, NV_ADMIN_DATA_N_ENTRIES * sizeof(adminSectorAddresses[0]));

            if (whichAdminSector >= 0 && whichAdminSector < NV_ADMIN_DATA_N_ENTRIES)
            {
                uint32_t pageAddressInBytes = adminSectorAddresses[whichAdminSector];
                return read_device_pages(dev, pageAddressInBytes, data, SPI_FLASH_PAGES_PER_SECTOR);
            }

            return false;
        }

        ds_calibration read_calibration_and_rectification_parameters(const uint8_t(&flash_data_buffer)[SPI_FLASH_SECTOR_SIZE_IN_BYTES])
        {
            struct RectifiedIntrinsics
            {
                big_endian<float> rfx, rfy;
                big_endian<float> rpx, rpy;
                big_endian<uint32_t> rw, rh;
                operator rs_intrinsics () const { return{ (int)rw, (int)rh, rpx, rpy, rfx, rfy, RS_DISTORTION_NONE, {0,0,0,0,0} }; }
            };

            ds_calibration cameraCalib = {};
            cameraCalib.version = reinterpret_cast<const big_endian<uint32_t> &>(flash_data_buffer);
            if (cameraCalib.version == 0)
            {
                struct UnrectifiedIntrinsicsV0
                {
                    big_endian<float> fx, fy;
                    big_endian<float> px, py;
                    big_endian<double> k[5];
                    big_endian<uint32_t> w, h;
                    operator rs_intrinsics () const { return{ (int)w, (int)h, px, py, fx, fy, RS_DISTORTION_MODIFIED_BROWN_CONRADY, {(float)k[0],(float)k[1],(float)k[2],(float)k[3],(float)k[4]} }; }
                };

                struct CameraCalibrationParametersV0
                {
                    enum { MAX_INTRIN_RIGHT = 2 };      ///< Max number right cameras supported (e.g. one or two, two would support a multi-baseline unit)
                    enum { MAX_INTRIN_THIRD = 3 };      ///< Max number native resolutions the third camera can have (e.g. 1920x1080 and 640x480)
                    enum { MAX_MODES_LR = 4 };    ///< Max number rectified LR resolution modes the structure supports (e.g. 640x480, 492x372 and 332x252)
                    enum { MAX_MODES_THIRD = 4 }; ///< Max number rectified Third resolution modes the structure supports (e.g. 1920x1080, 1280x720, 640x480 and 320x240)
                    UnrectifiedIntrinsicsV0 intrinsicsThird[MAX_INTRIN_THIRD];
                    RectifiedIntrinsics modesLR[MAX_INTRIN_RIGHT][MAX_MODES_LR];
                    RectifiedIntrinsics modesThird[MAX_INTRIN_RIGHT][MAX_INTRIN_THIRD][MAX_MODES_THIRD];
                    big_endian<double> Rthird[MAX_INTRIN_RIGHT][9];
                    big_endian<float> B[MAX_INTRIN_RIGHT];
                    big_endian<float> T[MAX_INTRIN_RIGHT][3];
                };

                const auto & calib = reinterpret_cast<const CameraCalibrationParametersV0 &>(flash_data_buffer);
                for (int i = 0; i < 3; ++i) cameraCalib.modesLR[i] = calib.modesLR[0][i];
                for (int i = 0; i < 2; ++i)
                {
                    cameraCalib.intrinsicsThird[i] = calib.intrinsicsThird[i];
                    for (int j = 0; j < 2; ++j) cameraCalib.modesThird[i][j] = calib.modesThird[0][i][j];
                }
                for (int i = 0; i < 9; ++i) cameraCalib.Rthird[i] = static_cast<float>(calib.Rthird[0][i]);
                for (int i = 0; i < 3; ++i) cameraCalib.T[i] = calib.T[0][i];
                cameraCalib.B = calib.B[0];
            }
            else if (cameraCalib.version == 1 || cameraCalib.version == 2)
            {
                struct UnrectifiedIntrinsicsV2
                {
                    big_endian<float> fx, fy;
                    big_endian<float> px, py;
                    big_endian<float> k[5];
                    big_endian<uint32_t> w, h;
                    operator rs_intrinsics () const { return{ (int)w, (int)h, px, py, fx, fy, RS_DISTORTION_MODIFIED_BROWN_CONRADY, {k[0],k[1],k[2],k[3],k[4]} }; }
                };

                struct CameraCalibrationParametersV2
                {
                    enum { MAX_INTRIN_RIGHT = 2 }; // Max number right cameras supported (e.g. one or two, two would support a multi-baseline unit)
                    enum { MAX_INTRIN_THIRD = 3 }; // Max number native resolutions the third camera can have (e.g. 1920x1080 and 640x480)
                    enum { MAX_MODES_LR = 4 }; // Max number rectified LR resolution modes the structure supports (e.g. 640x480, 492x372 and 332x252)
                    enum { MAX_MODES_THIRD = 3 }; // Max number rectified Third resolution modes the structure supports (e.g. 1920x1080, 1280x720, etc)

                    UnrectifiedIntrinsicsV2 intrinsicsThird[MAX_INTRIN_THIRD];
                    RectifiedIntrinsics modesLR[MAX_INTRIN_RIGHT][MAX_MODES_LR];
                    RectifiedIntrinsics modesThird[MAX_INTRIN_RIGHT][MAX_INTRIN_THIRD][MAX_MODES_THIRD];
                    big_endian<double> Rthird[MAX_INTRIN_RIGHT][9];
                    big_endian<float> B[MAX_INTRIN_RIGHT];
                    big_endian<float> T[MAX_INTRIN_RIGHT][3];
                };

                const auto & calib = reinterpret_cast<const CameraCalibrationParametersV2 &>(flash_data_buffer);
                for (int i = 0; i < 3; ++i) cameraCalib.modesLR[i] = calib.modesLR[0][i];
                for (int i = 0; i < 2; ++i)
                {
                    cameraCalib.intrinsicsThird[i] = calib.intrinsicsThird[i];
                    for (int j = 0; j < 2; ++j) cameraCalib.modesThird[i][j] = calib.modesThird[0][i][j];
                }
                for (int i = 0; i < 9; ++i) cameraCalib.Rthird[i] = calib.Rthird[0][i];
                for (int i = 0; i < 3; ++i) cameraCalib.T[i] = calib.T[0][i];
                cameraCalib.B = calib.B[0];
            }
            else
            {
                throw std::runtime_error(to_string() << "Unsupported calibration version: " << cameraCalib.version);
            }

            return cameraCalib;
        }

        ds_head_content read_camera_head_contents(const uint8_t(&flash_data_buffer)[SPI_FLASH_SECTOR_SIZE_IN_BYTES], uint32_t & serial_number)
        {
            //auto header = reinterpret_cast<const CameraHeadContents &>(flash_data_buffer[CAM_INFO_BLOCK_LEN]);
            ds_head_content head_content = reinterpret_cast<const ds_head_content &>(flash_data_buffer[CAM_INFO_BLOCK_LEN]);

            serial_number = head_content.serial_number;

            LOG_INFO("Serial number                       = " << head_content.serial_number);
            LOG_INFO("Model number                        = " << head_content.imager_model_number);
            LOG_INFO("Revision number                     = " << head_content.module_revision_number);
            LOG_INFO("Camera head contents version        = " << head_content.camera_head_contents_version);
            if (head_content.camera_head_contents_version != ds_head_content::DS_HEADER_VERSION_NUMBER) LOG_WARNING("Camera head contents version != 12, data may be missing/incorrect");
            LOG_INFO("Module version                      = " << (int)head_content.module_version << "." << (int)head_content.module_major_version << "." << (int)head_content.module_minor_version << "." << (int)head_content.module_skew_version);
            LOG_INFO("OEM ID                              = " << head_content.oem_id);
            LOG_INFO("Lens type for left/right imagers    = " << head_content.lens_type);
            LOG_INFO("Lens type for third imager          = " << head_content.lens_type_third_imager);
            LOG_INFO("Lens coating for left/right imagers = " << head_content.lens_coating_type);
            LOG_INFO("Lens coating for third imager       = " << head_content.lens_coating_type_third_imager);
            LOG_INFO("Nominal baseline (left to right)    = " << head_content.nominal_baseline << " mm");
            LOG_INFO("Nominal baseline (left to third)    = " << head_content.nominal_baseline_third_imager << " mm");
            LOG_INFO("Built on "        << time_to_string(head_content.build_date)          << " UTC");
            LOG_INFO("Calibrated on "   << time_to_string(head_content.calibration_date)    << " UTC");
            return head_content;
        }

        ds_info read_camera_info(uvc::device & device)
        {
            uint8_t flashDataBuffer[SPI_FLASH_SECTOR_SIZE_IN_BYTES];
            if (!read_admin_sector(device, flashDataBuffer, NV_CALIBRATION_DATA_ADDRESS_INDEX)) throw std::runtime_error("Could not read calibration sector");

            ds_info cam_info = {};

            try
            {
                cam_info.calibration = read_calibration_and_rectification_parameters(flashDataBuffer);
                cam_info.head_content = read_camera_head_contents(flashDataBuffer, cam_info.calibration.serial_number);
            }
            catch (std::runtime_error &err){ LOG_ERROR("Failed to read camera info " << err.what()); }
            catch (...){ LOG_ERROR("Failed to read camera info, may not work correctly"); }

            return cam_info;
        }

        std::string read_firmware_version(uvc::device & device)
        {
            auto response = send_command_and_receive_response(device, CommandResponsePacket(command::get_fwrevision));
            return reinterpret_cast<const char *>(response.reserved);
        }

        std::string read_isp_firmware_version(uvc::device & device)
        {
            auto response = send_command_and_receive_response(device, CommandResponsePacket(command::get_fwrevision));
            return to_string() << "0x" << std::hex << response.reserved[4];
        }

    } // namespace rsimpl::ds
}


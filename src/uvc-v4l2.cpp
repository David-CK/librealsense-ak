// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "uvc.h"

#include <cstring>
#include <fstream>
#include <regex>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <linux/videodev2.h>
#include <libusb.h>

//#include <sstream>
//#include <iostream>
//#include <sys/mman.h>
//#include <cstdio>
//#include <string>
//#include <cstdlib>
//#include <cassert>

//#include <algorithm>
//#include <functional>
//#include <sstream>
//#include <fstream>
//#include <thread>

//#include <utility>
//#include <chrono>
//#include <limits.h>
//#include <errno.h>

namespace rsimpl
{
    namespace uvc
    {
        static void throw_error(const char * s)
        {
            std::ostringstream ss;
            ss << s << " error " << errno << ", " << strerror(errno);
            throw std::runtime_error(ss.str());
        }

        static int xioctl(int fh, int request, void *arg)
        {
            int r;
            do {
                r = ioctl(fh, request, arg);
            } while (r < 0 && errno == EINTR);
            return r;
        }

        struct context
        {
            libusb_context * usb_context;

            context()
            {
                int status = libusb_init(&usb_context);
                //if(status < 0) throw std::runtime_error(to_string() << "libusb_init(...) returned " << libusb_error_name(status));
            }
        };

        struct subdevice
        {
            std::string dev_name;
            int busnum, devnum, parent_devnum;
            int vid, pid, mi;
            int fd;
            subdevice(const std::string & name) : dev_name("/dev/" + name)
            {
                struct stat st;
                if(stat(dev_name.c_str(), &st) < 0)
                {
                    std::ostringstream ss; ss << "Cannot identify '" << dev_name << "': " << errno << ", " << strerror(errno);
                    throw std::runtime_error(ss.str());
                }
                if(!S_ISCHR(st.st_mode)) throw std::runtime_error(dev_name + " is no device");

                // Search directory and up to three parent directories to find busnum/devnum
                std::ostringstream ss; ss << "/sys/dev/char/" << major(st.st_rdev) << ":" << minor(st.st_rdev) << "/device/";
                auto path = ss.str();
                bool good = false;
                for(int i=0; i<=3; ++i)
                {
                    if(std::ifstream(path + "busnum") >> busnum)
                    {
                        if(std::ifstream(path + "devnum") >> devnum)
                        {
                            if(std::ifstream(path + "../devnum") >> parent_devnum)
                            {
                                good = true;
                                break;
                            }
                        }
                    }
                    path += "../";
                }
                if(!good) throw std::runtime_error("Failed to read busnum/devnum");

                std::string modalias;
                if(!(std::ifstream("/sys/class/video4linux/" + name + "/device/modalias") >> modalias))
                    throw std::runtime_error("Failed to read modalias");
                if(modalias.size() < 14 || modalias.substr(0,5) != "usb:v" || modalias[9] != 'p')
                    throw std::runtime_error("Not a usb format modalias");
                if(!(std::istringstream(modalias.substr(5,4)) >> std::hex >> vid))
                    throw std::runtime_error("Failed to read vendor ID");
                if(!(std::istringstream(modalias.substr(10,4)) >> std::hex >> pid))
                    throw std::runtime_error("Failed to read product ID");
                if(!(std::ifstream("/sys/class/video4linux/" + name + "/device/bInterfaceNumber") >> std::hex >> mi))
                    throw std::runtime_error("Failed to read interface number");

                fd = open(dev_name.c_str(), O_RDWR | O_NONBLOCK, 0);
                if(fd < 0)
                {
                    std::ostringstream ss; ss << "Cannot open '" << dev_name << "': " << errno << ", " << strerror(errno);
                    throw std::runtime_error(ss.str());
                }

                v4l2_capability cap = {};
                if(xioctl(fd, VIDIOC_QUERYCAP, &cap) < 0)
                {
                    if(errno == EINVAL) throw std::runtime_error(dev_name + " is no V4L2 device");
                    else throw_error("VIDIOC_QUERYCAP");
                }
                if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) throw std::runtime_error(dev_name + " is no video capture device");
                if(!(cap.capabilities & V4L2_CAP_STREAMING)) throw std::runtime_error(dev_name + " does not support streaming I/O");

                // Select video input, video standard and tune here.
                v4l2_cropcap cropcap = {};
                cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if(xioctl(fd, VIDIOC_CROPCAP, &cropcap) == 0)
                {
                    v4l2_crop crop = {};
                    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    crop.c = cropcap.defrect; // reset to default
                    if(xioctl(fd, VIDIOC_S_CROP, &crop) < 0)
                    {
                        switch (errno)
                        {
                        case EINVAL: break; // Cropping not supported
                        default: break; // Errors ignored
                        }
                    }
                } else {} // Errors ignored
            }

            int get_vid() const { return vid; }
            int get_pid() const { return pid; }

            void get_control(const extension_unit & xu, uint8_t control, void * data, size_t size)
            {
            uvc_xu_control_query q = {static_cast<uint8_t>(xu.unit), control, UVC_GET_CUR, static_cast<uint16_t>(size), reinterpret_cast<uint8_t *>(data)};
                if(xioctl(fd, UVCIOC_CTRL_QUERY, &q) < 0) throw_error("UVCIOC_CTRL_QUERY:UVC_GET_CUR");
            }

            void set_control(const extension_unit & xu, uint8_t control, void * data, size_t size)
            {
            uvc_xu_control_query q = {static_cast<uint8_t>(xu.unit), control, UVC_SET_CUR, static_cast<uint16_t>(size), reinterpret_cast<uint8_t *>(data)};
                if(xioctl(fd, UVCIOC_CTRL_QUERY, &q) < 0) throw_error("UVCIOC_CTRL_QUERY:UVC_SET_CUR");
            }
        };

        struct device
        {
            std::vector<std::unique_ptr<subdevice>> subdevices;

            libusb_device * usb_device;
        };

        int get_vendor_id(const device & device) { return device.subdevices[0]->get_vid(); }
        int get_product_id(const device & device) { return device.subdevices[0]->get_pid(); }

        void get_control(const device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len)
        {
            device.subdevices[xu.subdevice]->get_control(xu, ctrl, data, len);
        }
        void set_control(device & device, const extension_unit & xu, uint8_t ctrl, void * data, int len)
        {
            device.subdevices[xu.subdevice]->set_control(xu, ctrl, data, len);
        }

        std::shared_ptr<context> create_context()
        {
            return std::make_shared<context>();
        }

        std::vector<std::shared_ptr<device>> query_devices(std::shared_ptr<context> context)
        {
            // Check if the uvcvideo kernel module is loaded
            std::ifstream modules("/proc/modules");
            std::string modulesline;
            std::regex regex("uvcvideo.* - Live.*");
            std::smatch match;
            bool module_found = false;


            while(std::getline(modules,modulesline) && !module_found)
            {
                module_found = std::regex_match(modulesline, match, regex);
            }

            if(!module_found)
            {
                throw std::runtime_error("uvcvideo kernel module is not loaded");
            }


            // Enumerate all subdevices present on the system
            std::vector<std::unique_ptr<subdevice>> subdevices;
            DIR * dir = opendir("/sys/class/video4linux");
            if(!dir) throw std::runtime_error("Cannot access /sys/class/video4linux");
            while (dirent * entry = readdir(dir))
            {
                std::string name = entry->d_name;
                if(name == "." || name == "..") continue;

                // Resolve a pathname to ignore virtual video devices
                std::string path = "/sys/class/video4linux/" + name;
                char buff[PATH_MAX];
                ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
                if (len != -1)
                {
                    buff[len] = '\0';
                    std::string real_path = std::string(buff);
                    if (real_path.find("virtual") != std::string::npos)
                    {
                        continue;
                    }
                }

                try
                {
                    std::unique_ptr<subdevice> sub(new subdevice(name));
                    subdevices.push_back(move(sub));
                }
                catch(const std::exception & e)
                {
                    LOG_INFO("Not a USB video device: " << e.what());
                }
            }
            closedir(dir);

            // Note: Subdevices of a given device may not be contiguous. We can test our grouping/sorting logic by calling random_shuffle.
            // std::random_shuffle(begin(subdevices), end(subdevices));

            // Group subdevices by busnum/devnum
            std::vector<std::shared_ptr<device>> devices;
            for(auto & sub : subdevices)
            {
                bool is_new_device = true;
                for(auto & dev: devices)
                {
                    if(sub->busnum == dev->subdevices[0]->busnum && sub->devnum == dev->subdevices[0]->devnum)
                    {
                       dev->subdevices.push_back(move(sub));
                       is_new_device = false;
                       break;
                    }
                }
                if(is_new_device)
                {
                    if(sub->vid == VID_INTEL_CAMERA && sub->pid == ZR300_FISHEYE_PID)  // avoid inserting fisheye camera as a device
                        continue;
                    devices.push_back(std::make_shared<device>());
                    devices.back()->subdevices.push_back(move(sub));
                }
            }

            // Sort subdevices within each device by multiple-interface index
            for(auto & dev : devices)
            {
                std::sort(begin(dev->subdevices), end(dev->subdevices), [](const std::unique_ptr<subdevice> & a, const std::unique_ptr<subdevice> & b)
                {
                    return a->mi < b->mi;
                });
            }

            // Insert fisheye camera as subDevice of ZR300
            for(auto & sub : subdevices)
            {
                if (!sub)
                    continue;
                for(auto & dev : devices)
                {
                    if (dev->subdevices[0]->vid == VID_INTEL_CAMERA && dev->subdevices[0]->pid == ZR300_CX3_PID &&
                        sub->vid == VID_INTEL_CAMERA && sub->pid == ZR300_FISHEYE_PID && dev->subdevices[0]->parent_devnum == sub->parent_devnum)
                    {
                        dev->subdevices.push_back(move(sub));
                        break;
                    }
                }
            }


            // Obtain libusb_device_handle for each device
            libusb_device ** list;
            int status = libusb_get_device_list(context->usb_context, &list);
            if(status < 0) throw std::runtime_error(to_string() << "libusb_get_device_list(...) returned " << libusb_error_name(status));
            for(int i=0; list[i]; ++i)
            {
                libusb_device * usb_device = list[i];
                int busnum = libusb_get_bus_number(usb_device);
                int devnum = libusb_get_device_address(usb_device);

                // Look for a video device whose busnum/devnum matches this USB device
                for(auto & dev : devices)
                {
                    if (dev->subdevices.size() >=4)      // Make sure that four subdevices present
                    {
                        auto parent_device = libusb_get_parent(usb_device);
                        if (parent_device)
                        {
                            int parent_devnum = libusb_get_device_address(libusb_get_parent(usb_device));
                            // First, handle the special case of FishEye
                            bool bFishEyeDevice = ((busnum == dev->subdevices[3]->busnum) && (parent_devnum == dev->subdevices[3]->parent_devnum));
                            if (bFishEyeDevice && !dev->usb_device)
                            {
                                dev->usb_device = usb_device;
                                libusb_ref_device(usb_device);
                                break;
                            }
                        }
                    }
                    if (busnum == dev->subdevices[0]->busnum && devnum == dev->subdevices[0]->devnum)
                    {
                        if (!dev->usb_device) // do not override previous configuration
                        {
                            dev->usb_device = usb_device;
                            libusb_ref_device(usb_device);
                            break;
                        }
                    }
                }
            }
            libusb_free_device_list(list, 1);

            return devices;
        }
    }
}


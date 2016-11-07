// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "uvc.h"
#include <fstream>
#include <regex>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

//#include <fcntl.h>
//#include <sstream>

//inc
#include <linux/usb/video.h>
#include <linux/uvcvideo.h>
#include <linux/videodev2.h>
//#include <libusb.h>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>

#include <utility>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>
//inc

namespace rsimpl
{
    namespace uvc
    {
        //std::shared_ptr<context> create_context()
        //{
            //return std::make_shared<context>();
        //}
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
                std::ostringstream ss; ss << "/sys/dev/char/" << major(st.st_rdev) << ":" << minor(st.st_rdev) << "/device/";
                printf("path = ");//ck
                std::cout << ss.str() << std::endl;//ck
                auto path = ss.str();
                bool good = false;
                for(int i=0; i<=3; ++i)
                {
                    std::cout << path << std::endl;//ck
                                printf(">>>>%d-%d-%d-%d-\n", i, busnum, devnum, parent_devnum);
                    if(std::ifstream(path + "busnum") >> busnum)
                    {
                                printf(">>>>%d-%d-%d-%d-\n", i, busnum, devnum, parent_devnum);
                        if(std::ifstream(path + "devnum") >> devnum)
                        {
                                printf(">>>>%d-%d-%d-%d-\n", i, busnum, devnum, parent_devnum);
                            if(std::ifstream(path + "../devnum") >> parent_devnum)
                            {
                                printf("ggggggggggood----\n");
                                printf(">>>>%d-%d-%d-%d-\n", i, busnum, devnum, parent_devnum);
                                good = true;
                                break;
                            }
                        }
                    }
                    path += "../";
                }
                if(!good) throw std::runtime_error("Failed to read busnum/devnum");
                if(good)
                {
                    printf("good----\n");
                }
                else {
                    printf("good good----\n");
                }
                std::string modalias;
                if(!(std::ifstream("/sys/class/video4linux" + name + "/device/modalias") >> modalias))
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

                v4l2_cropcap cropcap = {};
                cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                if(xioctl(fd, VIDIOC_CROPCAP, &cropcap) == 0)
                {
                    v4l2_crop crop = {};
                    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    crop.c = cropcap.defrect;
                    if(xioctl(fd, VIDIOC_S_CROP, &crop) < 0)
                    {
                        switch (errno)
                        {
                        case EINVAL: break;
                        default: break;
                        }
                    }
                } else {}
                close(fd);
            }
        };
        struct device
        {
        };
        std::vector<std::shared_ptr<device>> query_devices()
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
                printf(">>>>module_found\n");
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
                printf(">>>>path = %s\n", path.c_str());
                char buff[PATH_MAX];
                ssize_t len = ::readlink(path.c_str(), buff, sizeof(buff)-1);
                printf(">>>>len = %ld\n", len);
                if (len != -1)
                {
                    buff[len] = '\0';
                    printf(">>>>buff= %s\n", buff);
                    std::string real_path = std::string(buff);
                    printf("find = %ld\n", real_path.find("virtual"));
                    if (real_path.find("virtual") != std::string::npos)
                    {
                        printf(">>>>continue\n");
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
                    printf(">>>>Not a USB video device\n");
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
                for(auto & dev: devices)
                {
                    //if(sub->busnum == dev->subdevices[0]->busnum && sub->devnum == dev->subdevices[0]->devnum)
                    //{
                    //}
                }
            }
            printf("------------------\n");
            return devices;
        }
    }
}

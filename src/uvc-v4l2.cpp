// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

#include "uvc.h"
#include <fstream>
#include <regex>
#include <dirent.h>
#include <unistd.h>

//#include <fcntl.h>
//#include <sstream>

namespace rsimpl
{
    namespace uvc
    {
        //std::shared_ptr<context> create_context()
        //{
            //return std::make_shared<context>();
        //}
        struct subdevice
        {
            std::string dev_name;
            subdevice(const std::string & name) : dev_name("/dev/" + name)
            {
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
                        continue;
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
            std::vector<std::shared_ptr<device>> devices;
            printf("------------------\n");
            return devices;
        }
    }
}

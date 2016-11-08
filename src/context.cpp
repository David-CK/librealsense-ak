/*
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

//#include <array>
//#include <string>
*/

#include "uvc.h"
#include "context.h"
//inc
#include "iostream"
//inc
rs_context_base::rs_context_base()
{
    int i = 0;
    //rsimpl::uvc::create_context();
    for(auto device : rsimpl::uvc::query_devices())
    {
        printf("iiiiiiiiiii = %d\n", i);
        std::cout << get_vendor_id(*device);
        printf("\n");
        std::cout << get_product_id(*device);
        printf("\n");
        LOG_INFO("UVC device detected with VID = 0x" << std::hex << get_vendor_id(*device) << " PID = 0x" << get_product_id(*device));
        printf("i = %d\n", i++);
        std::shared_ptr<rs_device> rs_dev;
    }
}

rs_context* rs_context_base::instance = nullptr;
rs_context* rs_context_base::acquire_instance()
{
    instance = new rs_context_base();
    return instance;
}
size_t rs_context_base::get_device_count() const
{
    return 10;
    //return devices.size();
}
rs_device* rs_context_base::get_device(int index) const
{
    //return devices[index].get();
}

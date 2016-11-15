/*
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

//#include <array>
//#include <string>
*/

//#include "uvc.h"
#include "context.h"
#if 0
//inc
#include "iostream"
//inc
#endif
#include "r200.h"
rs_context_base::rs_context_base()
{
    context = rsimpl::uvc::create_context();

    for(auto device : rsimpl::uvc::query_devices(context))
    {
        LOG_INFO("UVC device detected with VID = 0x" << std::hex << get_vendor_id(*device) << " PID = 0x" << get_product_id(*device));
        if (get_vendor_id(*device) != VID_INTEL_CAMERA)
            continue;

        std::shared_ptr<rs_device> rs_dev;

        switch(get_product_id(*device))
        {
            case R200_PRODUCT_ID:  rs_dev = rsimpl::make_r200_device(device); break;
            //case LR200_PRODUCT_ID: rs_dev = rsimpl::make_lr200_device(device); break;
            //case ZR300_PRODUCT_ID: rs_dev = rsimpl::make_zr300_device(device); break;
            //case F200_PRODUCT_ID:  rs_dev = rsimpl::make_f200_device(device); break;
            //case SR300_PRODUCT_ID: rs_dev = rsimpl::make_sr300_device(device); break;
        }
    }
}

rs_context* rs_context_base::instance = nullptr;

rs_context* rs_context_base::acquire_instance()
{
    instance = new rs_context_base();
    return instance;
}
#if 0
size_t rs_context_base::get_device_count() const
{
    return 10;
    //return devices.size();
}
rs_device* rs_context_base::get_device(int index) const
{
    //return devices[index].get();
}
#endif

/*
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

//#include <array>
//#include <string>
*/

#include "uvc.h"
#include "context.h"
rs_context_base::rs_context_base()
{
    int i = 0;
    //rsimpl::uvc::create_context();
    for(auto device : rsimpl::uvc::query_devices())
    {
        printf("i = %d\n", i++);
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

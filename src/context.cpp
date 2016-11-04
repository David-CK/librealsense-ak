/*
// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2015 Intel Corporation. All Rights Reserved.

//#include <array>
//#include <string>
//#include "context.h"
#include <stdio.h>

void ars_create_context(void)
{
    printf("hello realsense!\n");
}
*/

/*
size_t rs_context_base::get_device_count() const
{
    //return devices.size();
}

rs_device* rs_context_base::get_device(int index) const
{
    //return devices[index].get();
}
*/

#include "uvc.h"
#include "context.h"
rs_context_base::rs_context_base()
{
    rsimpl::uvc::create_context();
}

rs_context* rs_context_base::acquire_instance()
{
    new rs_context_base();
}

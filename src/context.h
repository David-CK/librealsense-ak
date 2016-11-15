#ifndef LIBREALSENSE_CONTEXT_H
#define LIBREALSENSE_CONTEXT_H

#include "types.h"
#include "uvc.h"

struct rs_context_base : rs_context
{
    std::shared_ptr<rsimpl::uvc::context>           context;
    //std::vector<std::shared_ptr<rs_device>>         devices;
    //size_t                                          get_device_count() const override;
    //rs_device *                                     get_device(int index) const override;
                                                    rs_context_base();
    static rs_context*                              acquire_instance();
private:
    static rs_context*                              instance;
};

#endif


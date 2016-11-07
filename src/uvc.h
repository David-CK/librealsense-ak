#include "types.h"
#include <memory>
//inc
#include <functional>
#include <memory>
//inc

namespace rsimpl
{
    namespace uvc
    {
        //struct context;
        //std::shared_ptr<context> create_context();
        struct device;
        std::vector<std::shared_ptr<device>> query_devices();
    }
}


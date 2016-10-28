#include <cstddef>

struct rs_device
{
    virtual const char *                    get_name() const = 0;
};

struct rs_context
{
    virtual size_t                          get_device_count() const = 0;
    virtual rs_device *                     get_device(int index) const = 0;
};


#include "types.h"

struct rs_context_base : rs_context
{
    std::vector<std::shared_ptr<rs_device>>         devices;
    size_t                                          get_device_count() const override;
    rs_device *                                     get_device(int index) const override;
};


#include <memory>

struct rs_device_base : rs_device
{
//protected:
 //   rsimpl::device_config                       config;
public:
    const char *                                get_name() const override { /*return config.info.name.c_str();*/ }
    const char *                                get_serial() const override {/**/}
    const char *                                get_firmware_version() const override {/**/}
};

struct rs_device_base : rs_device
{
public:
    const char *                                get_name() const override { return config.info.name.c_str(); }//CK
};


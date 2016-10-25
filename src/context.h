struct rs_context_base : rs_context
{
    size_t                                          get_device_count() const override;
    rs_device *                                     get_device(int index) const override;
};


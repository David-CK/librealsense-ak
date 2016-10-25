size_t rs_context_base::get_device_count() const
{
    return devices.size();//CK
}

rs_device* rs_context_base::get_device(int index) const
{
    return devices[index].get();//CK
}


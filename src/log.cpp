#include "types.h"

namespace rsimpl {
    class logger_type {
    private:
        rs_log_severity minimum_log_severity = RS_LOG_SEVERITY_NONE;
    public:
        rs_log_severity get_minimum_severity() { return minimum_log_severity; }
        void log(rs_log_severity severity, const std::string & message)
        {
        }
    };

    static logger_type logger;
}

rs_log_severity rsimpl::get_minimum_severity(void)
{
    return logger.get_minimum_severity();
}

void rsimpl::log(rs_log_severity severity, const std::string & message)
{
    logger.log(severity, message);
}


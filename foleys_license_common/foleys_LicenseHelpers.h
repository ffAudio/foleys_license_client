//
// Created by Daniel Walz on 11.03.25.
//

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEHELPERS_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEHELPERS_H

#include "foleys_LicenseDefines.h"

#include <sstream>
#include <fstream>
#include <iomanip>

namespace foleys::Helpers
{

static inline time_t decodeDateTime (const std::string& timeString, const char* formatString)
{
    std::tm            tm {};
    std::istringstream ss (timeString);
    ss >> std::get_time (&tm, formatString);
    return std::mktime (&tm);
}

static inline std::string formatDateTime (std::time_t date, std::string_view format)
{
    char buff[20];
    strftime (buff, 20, format.data(), localtime (&date));
    return { buff };
}

}  // namespace foleys::Helpers

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEHELPERS_H

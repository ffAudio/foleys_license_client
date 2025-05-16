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
    std::tm tm {};
#ifdef _WIN32
    localtime_s (&tm, &date);
#else
    localtime_r (&date, &tm);
#endif

    std::array<char, 20> buff {0};
    strftime (buff.data(), sizeof(buff), format.data(), &tm);
    return { buff.data() };
}

}  // namespace foleys::Helpers

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEHELPERS_H

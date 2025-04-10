/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
 ********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

 *******************************************************************************/

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDEFINES_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDEFINES_H

#include <string>
#include <atomic>
#include <optional>
#include <vector>
#include <functional>
#include <memory>
#include <ctime>

#define FOLEYS_DISABLE_COPY(className)                \
    className (const className&)            = delete; \
    className& operator= (const className&) = delete;

/**
 * Add this FOLEYS_DECLARE_SAFEPOINTER macro to your observer declaration. This allows to check if observers were
 * deleted while iterating the list (dangling).
 */
#define FOLEYS_DECLARE_SAFEPOINTER(className)        \
    using SafePointer = std::shared_ptr<className*>; \
    virtual ~className()                             \
    {                                                \
        (*reference) = nullptr;                      \
    }                                                \
    SafePointer getSafePointer()                     \
    {                                                \
        return reference;                            \
    }                                                \
                                                     \
private:                                             \
    SafePointer reference { std::make_shared<className*> (this) };

#include <string>

namespace foleys::LicenseDefines
{

enum class Error
{
    NoError = 0,
    ServerNotAvailable,
    ServerAnswerInvalid,
    ServerError,
    CouldNotSave,
    CouldNotRead,
    HardwareMismatch,
};

enum class Status
{
    // Make sure to keep synchronised with the server!
    Unchecked        = 0,  //< Initial state
    Activated        = 1,  //< Activated and can be run
    NotActivated     = 2,  //< But can be activated
    Expired          = 3,  //< License expired
    Deactivated      = 4,  //< License was manually deactivated
    NoActivationLeft = 5,  //< tried to activate but no activations left
};

}  // namespace foleys::LicenseDefines

namespace foleys
{
struct Activation
{
    size_t      index = 0;
    std::string computer;
    std::string user;
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSEDEFINES_H

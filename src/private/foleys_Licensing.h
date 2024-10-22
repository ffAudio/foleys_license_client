/********************************************************************************
    Copyright 2024 - Daniel Walz
    Foleys Finest Audio UG (haftungsbeschraenkt)
    Lobith Audio UG (haftungsbeschraenkt)
 ********************************************************************************

    This code is provided under the ISC license

    This code is provided as is. The authors disclaim all warranties.
    For details refer to the LICENSE.md

 *******************************************************************************/

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSING_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSING_H

namespace foleys::Licensing
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

struct Activation
{
    size_t      index = 0;
    std::string computer;
    std::string user;
};

}  // namespace foleys::Licensing


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSING_H

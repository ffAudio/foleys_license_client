/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/

// we link statically to sodium
#define SODIUM_STATIC 1
#if _WIN32
    #pragma comment(lib, "libsodium.lib")
#endif

#include "foleys_Crypto.h"
#include "foleys_LicenseData.h"
#include "foleys_LicenseDefines.h"

#include "foleys_license_common/libsodium/include/sodium.h"

#include "foleys_license_lib/choc_Base64.h"


namespace foleys
{

std::string Crypto::decrypt (std::string_view data)
{
    std::vector<unsigned char> binary;
    if (!choc::base64::decodeToContainer (binary, data))
        return {};

    std::vector<unsigned char> message (binary.size());
    if (crypto_box_open_easy (static_cast<unsigned char*> (message.data()), binary.data() + crypto_box_noncebytes(), binary.size() - crypto_box_noncebytes(),
                              binary.data(), LicenseData::publicKey, LicenseData::privateKey)
        != 0)
    {
        return {};
    }

    return { message.begin(), message.end() };
}

std::string Crypto::encrypt (std::string_view message)
{
    const auto                 numBytes = message.size() + crypto_box_noncebytes();
    std::vector<unsigned char> plain { message.begin(), message.end() };
    std::vector<unsigned char> binary (numBytes);

    randombytes_buf (binary.data(), crypto_box_noncebytes());

    if (crypto_box_easy (binary.data() + crypto_box_noncebytes(), plain.data(), plain.size(), binary.data(), LicenseData::publicKey, LicenseData::privateKey) != 0)
    {
        return {};
    }

    return choc::base64::encodeToString (binary.data(), binary.size());
}

}  // namespace foleys

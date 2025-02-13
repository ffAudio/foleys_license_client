/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_CRYPTO_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_CRYPTO_H

#include <string>

namespace foleys
{

struct Crypto
{
    /**
     * Decrypts a message. The cipher is encoded in base64 and starts with NONCE_BYTES of a random nonce
     * @param cipher
     * @return the plain text
     */
    static std::string decrypt (std::string_view cipher);

    /**
     * Encrypts a message. The returned cipher is wrapped into base64
     * @param message
     * @return a base64 encoded cipher
     */
    static std::string encrypt (std::string_view message);
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_CRYPTO_H

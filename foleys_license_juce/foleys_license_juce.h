/*
 ================================================================================

 Created by Daniel Walz on 13.02.25.

 BEGIN_JUCE_MODULE_DECLARATION

    ID:               foleys_license_juce
    vendor:           Foleys Finest Audio
    version:          1.0.0
    name:             Foleys license activation scheme
    description:      A client to request licenses fro a bespoke license server
    website:          http://www.foleysfinest.com/
    license:          ISC license

    dependencies:     juce_events
    OSXLibs:          sodium
    windowsLibs:      libsodium

    END_JUCE_MODULE_DECLARATION

 ================================================================================

    DISCLAIMER

    THE SOFTWARE IS PROVIDED “AS IS” AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
    AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
    INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
    LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
    OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

 ================================================================================
 */

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSE_JUCE_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSE_JUCE_H

#ifndef FOLEYS_LICENSING_HAS_JUCE
#define FOLEYS_LICENSING_HAS_JUCE 1
#endif

#include "juce_events/juce_events.h"

#include "GUI/foleys_PopupHolder.h"
#include "../src/foleys_License.h"

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_LICENSE_JUCE_H

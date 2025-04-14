//
// Created by Daniel Walz on 11.04.25.
//

#include "foleys_FileDragButton.h"
#include "foleys_license_common/foleys_License.h"

namespace foleys
{

void FileDragButton::mouseDrag (const juce::MouseEvent& event)
{
    if (!event.mouseWasDraggedSinceMouseDown())
        return;

    License license;

    auto file =
      juce::File::getSpecialLocation (juce::File::tempDirectory).getChildFile (juce::String (LicenseData::productName) + "-" + juce::SystemStats::getComputerName() + ".request");
    if (auto stream = file.createOutputStream())
    {
        auto text = license.getOfflineRequest();
        stream->setPosition (0);
        stream->truncate();
        stream->write (text.data(), text.size());
    }
    else
    {
        return;
    }

    juce::DragAndDropContainer::performExternalDragDropOfFiles ({ file.getFullPathName() }, false);
}


}  // namespace foleys

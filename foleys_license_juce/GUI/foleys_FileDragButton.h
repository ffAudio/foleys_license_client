//
// Created by Daniel Walz on 11.04.25.
//

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_FILEDRAGBUTTON_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_FILEDRAGBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace foleys
{

class FileDragButton : public juce::DrawableButton
{
public:
    using juce::DrawableButton::DrawableButton;
    void mouseDrag (const juce::MouseEvent& event) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FileDragButton)
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_FILEDRAGBUTTON_H

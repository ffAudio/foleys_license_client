/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_POPUPHOLDER_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_POPUPHOLDER_H

#include "juce_gui_basics/juce_gui_basics.h"

namespace foleys
{

class Popup : public juce::Component
{
public:
    using juce::Component::Component;

    std::function<void()> onCloseRequest;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Popup)
};


class PopupHolder : private juce::ComponentListener
{
public:
    explicit PopupHolder (juce::Component* parent) : parentComponent (parent) { parentComponent->addComponentListener (this); }

    ~PopupHolder() override
    {
        if (parentComponent)
            parentComponent->removeComponentListener (this);
    }

    void showPopup (std::unique_ptr<Popup> popupToShow)
    {
        if (!parentComponent)
            return;

        popup = std::move (popupToShow);
        parentComponent->addAndMakeVisible (popup.get());

        popup->setBounds (parentComponent->getLocalBounds());
        popup->onCloseRequest = [this] { closePopup(); };
    }

    void closePopup() { popup.reset(); }

    void componentMovedOrResized (juce::Component& component, [[maybe_unused]] bool wasMoved, [[maybe_unused]] bool wasResized) override
    {
        if (popup && parentComponent)
            popup->setBounds (component.getLocalBounds());
    }

private:
    juce::Component::SafePointer<juce::Component> parentComponent;
    std::unique_ptr<Popup>                        popup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PopupHolder)
};


}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_POPUPHOLDER_H

/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H

#include <vector>
#include <mutex>
#include <functional>


namespace foleys
{

/**
 * A list of observers to be notified of events.
 * @tparam ObserverType the observer interface that can be called through the list.
 */
template<typename ObserverType>
class ObserverList
{
public:
    ObserverList() = default;

    /**
     * Add an observer to the list of observers
     * @param observer the observer to add
     */
    void addObserver (ObserverType* observer)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        if (std::find (observers.begin(), observers.end(), observer) != observers.cend())
            return;

        observers.push_back (observer);
    }

    /**
     * Remove all occurrences of the observer from the list of observers
     * @param observer the observer to remove
     */
    void removeObserver (ObserverType* observer)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        observers.erase (std::remove_if (observers.begin(), observers.end(), [observer] (const ObserverType* o) { return o == observer; }), observers.end());
    }

    /**
     * Call the function on every observer. It will crash, if an observer got deleted in the meantime.
     * @param func the std::function to call
     */
    void callUnChecked (std::function<void (ObserverType&)> func)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        for (auto* o: observers)
            func (*o);
    }

    /**
     * Call the function on all observers. It uses safe pointers to avoid dangling objects. It doesn't protect against concurrent destruction though.
     * @param func The std::function to call
     */
    void call (std::function<void (ObserverType&)> func)
    {
        std::vector<typename ObserverType::SafePointer> references;

        {
            std::scoped_lock<std::mutex> guard (observerLock);
            for (auto* o: observers)
                references.push_back (o->getSafePointer());
        }

        for (auto& o: references)
            if (*o)
                func (**o);
    }

    ObserverList (const ObserverList&)            = delete;
    ObserverList& operator= (const ObserverList&) = delete;

private:
    std::mutex                 observerLock;
    std::vector<ObserverType*> observers;
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H

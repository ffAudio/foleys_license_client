//
// Created by Daniel Walz on 27.08.24.
//

#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H

#include <vector>
#include <mutex>
#include <functional>

namespace foleys
{

template<typename ObserverType>
class ObserverList
{
public:
    ObserverList() = default;

    void addObserver (ObserverType* observer)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        if (std::find (observers.begin(), observers.end(), observer) != observers.cend())
            return;

        observers.push_back (observer);
    }

    void removeObserver (ObserverType* observer)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        observers.erase (std::remove_if (observers.begin(), observers.end(), [observer] (const ObserverType* o) { return o == observer; }), observers.end());
    }

    void call (std::function<void (ObserverType&)> func)
    {
        std::scoped_lock<std::mutex> guard (observerLock);
        for (auto* o: observers)
            func (*o);
    }

    ObserverList (const ObserverList&)            = delete;
    ObserverList& operator= (const ObserverList&) = delete;

private:
    std::mutex                 observerLock;
    std::vector<ObserverType*> observers;
};

}  // namespace foleys


#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_OBSERVERS_H

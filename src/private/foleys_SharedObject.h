/********************************************************************************
Copyright 2024 - Daniel Walz
Foleys Finest Audio UG (haftungsbeschraenkt)
Lobith Audio UG (haftungsbeschraenkt)
********************************************************************************

This code is provided under the ISC license

This code is provided as is. The authors disclaim all warranties.
For details refer to the LICENSE.md

*******************************************************************************/


#ifndef FOLEYS_LICENSING_CLIENT_FOLEYS_SHAREDOBJECT_H
#define FOLEYS_LICENSING_CLIENT_FOLEYS_SHAREDOBJECT_H

#include <memory>


namespace foleys
{

template<typename ObjectType>
class SharedObject
{
public:
    SharedObject()  = default;
    ~SharedObject() = default;

    ObjectType* operator->() const { return sharedObject.get(); }
    ObjectType& operator*() const { return *sharedObject; }

    SharedObject& operator= (const SharedObject&) = delete;
    SharedObject& operator= (SharedObject&&)      = delete;

private:
    struct Wrapper
    {
        std::shared_ptr<ObjectType> get() { return ptr.lock(); }

        std::shared_ptr<ObjectType> getOrCreate()
        {
            if (auto p = get())
                return p;

            const auto newPointer = std::make_shared<ObjectType>();
            ptr                   = newPointer;
            return newPointer;
        }

        std::weak_ptr<ObjectType> ptr;
    };

    inline static Wrapper& wrapper()
    {
        static Wrapper wrapper;
        return wrapper;
    }

    std::shared_ptr<ObjectType> sharedObject = wrapper().getOrCreate();
};

}  // namespace foleys

#endif  // FOLEYS_LICENSING_CLIENT_FOLEYS_SHAREDOBJECT_H

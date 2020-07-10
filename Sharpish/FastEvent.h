#pragma once

#include <vector>
#include <functional>
#include "FastDelegate.h"
#include "CriticalSection.h"

namespace CS
{
    template<typename Signature>
    class EventTrigger;

    template<typename R, typename... Args>
    class EventTrigger<R(Args...)> : public std::vector<Delegate<R(Args...)>>
    {
        CriticalSection _csec;
        EventTrigger(const EventTrigger& copy) { }

    public:
        using Signature = R(Args...);

        EventTrigger() { }
        EventTrigger(EventTrigger&& copy) : std::vector(copy) { }

        void operator =(const EventTrigger& rhs) { static_assert(false, "Events cannot be assigned after initialization"); }

        EventTrigger& operator +=(const Delegate<Signature>& handler)
        {
            AutoLock lock(_csec);
            push_back(handler);
            return *this;
        }

        EventTrigger& operator -=(const Delegate<Signature>& handler)
        {
            AutoLock lock(_csec);
            auto finder = find(begin(), end(), handler);
            if (finder != end()) erase(finder);
            return *this;
        }

        void operator()(const Args&... args)
        {
            auto safe = SafeCopy();
            for (auto& i : safe)
                i(args...);
        }

        std::vector<Delegate<Signature>> SafeCopy()
        {
            AutoLock lock(_csec); // Presumed locked already

            std::vector<Delegate<Signature>> copy;
            bool shrunk = false;

            for (auto i = begin(); i < end(); i++)
            {
                if (!*i)
                {
                    shrunk = true;
                    continue;
                }

                copy.push_back(*i);
            }

            if (shrunk)
                assign(copy.begin(), copy.end());

            return copy;
        }
    };

    // Defines the publicly-exposed portion of the event
    template<class Signature, typename = std::enable_if_t<is_signature<Signature>>>
    class Event
    {
        EventTrigger<Signature>& _privateEvent;

    public:
        typedef EventTrigger<Signature> Trigger;

        Event(EventTrigger<Signature>& privateEvent) : _privateEvent(privateEvent) { }
        Event(Event&& copy) : _privateEvent(copy._privateEvent) { }

        template<typename T>
        void operator =(T rhs) { static_assert(false, "Events cannot be assigned"); }

        void operator +=(const Delegate<Signature>& handler) const { _privateEvent += handler; }
        void operator -=(const Delegate<Signature>& handler) const { _privateEvent -= handler; }
    };
}
#pragma once

#include <vector>
#include <functional>
#include "FastDelegate.h"
#include "CriticalSection.h"

namespace CS
{
    template<typename Signature>
    class EventTrigger;

    /**
     * \brief Trigger mechanism for events.
     * 
     * See CS::Event documentation for more details.
     * 
     * \see CS::Event
     */
    template<typename R, typename... Args>
    class EventTrigger<R(Args...)> : public std::vector<Delegate<R(Args...)>>
    {
        CriticalSection _csec;
        EventTrigger(const EventTrigger& copy) { }

    public:
        using Signature = R(Args...);

        /** Constructor. */
        EventTrigger() { }
        EventTrigger(EventTrigger&& copy) : std::vector(copy) { }

        void operator =(const EventTrigger& rhs) { static_assert(false, "Events cannot be assigned after initialization"); }

        /** Add a handler to the event */
        EventTrigger& operator +=(const Delegate<Signature>& handler)
        {
            AutoLock lock(_csec);
            push_back(handler);
            return *this;
        }

        /** Remove a handler from the event */
        EventTrigger& operator -=(const Delegate<Signature>& handler)
        {
            AutoLock lock(_csec);
            auto finder = find(begin(), end(), handler);
            if (finder != end()) erase(finder);
            return *this;
        }

        /** Fires the event */
        void operator()(const Args&... args)
        {
            auto safe = SafeCopy();
            for (auto& i : safe)
                i(args...);
        }

    private:
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

    /**
     * \brief Analog of a C# event.
     * 
     * The `Event` class cannot fire the event on its own. It must be constructed with a
     * corresponding `EventTrigger` object. The `Event` is designed to be declared as a
     * public or protected field, allowing a wider scope of code to register handlers. The
     * `EventTrigger` is designed to be a private or protected field. This gives the
     * declaring class exclusive permission to fire the event.
     * 
     * \note <b>Events cannot be copied. This is because there is no universally intuitive
     * way to manage event handlers during a copy operation, so it would inevitably lead to
     * unexpected behavior. For this reason events should only be declared on pointer-referenced
     * classes.</b>
     * 
     * \see CS::EventTrigger
     * \see CS::Delegate
     * 
     * Example of how to declare and fire a typical event:
     * 
     *     class comid("12345678-1234-1234-1234-123456789012") MyClass
     *         : public CS::ComObject<MyClass, CS::Object>
     *     {
     *     public:
     *         void FireMessage(CS::string msg) { _OnMessage(msg); }  // Fire OnMessage
     * 
     *         CS::Event<void(CS::string)> OnMessage { _OnMessage };  // Declare OnMessage and associate with trigger _OnMessage
     * 
     *     private:
     *         CS::EventTrigger<void(CS::string)> _OnMessage;         // Declare OnMessage trigger
     *     };
     * 
     * Example of how to register and unregister a handler for an event:
     * 
     *     CS::com_ptr<MyClass> myClass = CS::Make<MyClass>();
     *     CS::Delegate<void(CS::string)> handler = [](CS::string msg) { std::cout << msg << std::endl; };
     *     
     *     myClass->OnMessage += handler; // Registers handler
     *     myClass->OnMessage -= handler; // Unregisters handler
     */
    template<class Signature, typename = std::enable_if_t<is_signature<Signature>>>
    class Event
    {
        EventTrigger<Signature>& _trigger;

    public:
        typedef EventTrigger<Signature> Trigger;

        /**
         * Constructor.
         * 
         * \param trigger Reference to the trigger object to associate with the event.
         */
        Event(EventTrigger<Signature>& trigger) : _trigger(trigger) { }
        Event(Event&& copy) : _trigger(copy._trigger) { }

        template<typename T>
        void operator =(T rhs) { static_assert(false, "Events cannot be assigned"); }

        /** Add a handler to the event */
        void operator +=(const Delegate<Signature>& handler) const { _trigger += handler; }

        /** Remove a handler from the event */
        void operator -=(const Delegate<Signature>& handler) const { _trigger -= handler; }
    };
}
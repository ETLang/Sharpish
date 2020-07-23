#pragma once

#include <type_traits>

namespace CS
{
    namespace Details
    {
        // CNull is a filler class for templates that support variable-length lists of arguments.
        // Variadic templates are off limits until the XBox compiler catches up.
        class __declspec(uuid("00000000-0000-0000-0000-000000000000")) CNull { };

        template<typename T>
        struct IsSignature
        {
            static const bool value = false;
        };

        template<typename R, typename... Args>
        struct IsSignature<R(Args...)>
        {
            static const bool value = true;
        };

        template<typename T>
        struct SignatureTraits : public SignatureTraits<decltype(&T::operator())> { };

        template<typename T>
        struct SignatureTraits<T&> : public SignatureTraits<decltype(&T::operator())> { };

        template<typename T>
        struct SignatureTraits<T*&> : public SignatureTraits<T*> { };

        template<>
        struct SignatureTraits<nullptr_t> { };

        template<typename R, typename... Args>
        struct SignatureTraits<R(Args...)> { using Sig = R(Args...); };

        template<typename R, typename... Args>
        struct SignatureTraits<R(&)(Args...)> { using Sig = R(Args...); };

        template<typename R, typename... Args>
        struct SignatureTraits<R(*)(Args...)> { using Sig = R(Args...); };

        //template<typename R, typename... Args>
        //struct SignatureTraits<R(__cdecl *)(Args...)> { using Sig = R(Args...); };

        //template<typename R, typename... Args>
        //struct SignatureTraits<R(__fastcall *)(Args...)> { using Sig = R(Args...); };

        //template<typename R, typename... Args>
        //struct SignatureTraits<R(__vectorcall *)(Args...)> { using Sig = R(Args...); };

        template<typename R, typename C, typename... Args>
        struct SignatureTraits<R(__thiscall C::*)(Args...)> { using Sig = R(Args...); };

        template<typename R, typename... Args>
        struct SignatureTraits<R(Args...) const> { using Sig = R(Args...); };

        template<typename R, typename C, typename... Args>
        struct SignatureTraits<R(__thiscall C::*)(Args...) const> { using Sig = R(Args...); };

        template<typename R, typename... Args>
        struct SignatureTraits<R(Args...) volatile> { using Sig = R(Args...); };

        template<typename R, typename C, typename... Args>
        struct SignatureTraits<R(__thiscall C::*)(Args...) volatile> { using Sig = R(Args...); };

        template<typename R, typename... Args>
        struct SignatureTraits<R(Args...) const volatile> { using Sig = R(Args...); };

        template<typename R, typename C, typename... Args>
        struct SignatureTraits<R(__thiscall C::*)(Args...) const volatile> { using Sig = R(Args...); };

        struct FunctorWrapperBase
        {
            int Size;
            FunctorWrapperBase(int size, uint8_t* base) : Size(size)
            {
                memset((void*)(&Size + 1), 0, (base - (uint8_t*)&Size) - sizeof(int));
            }

            virtual ~FunctorWrapperBase() { }

            bool operator==(const FunctorWrapperBase& rhs) const
            {
                if (Size != rhs.Size) return false;
                return memcmp((void*)this, (void*)&rhs, Size) == 0;
            }

            bool operator<(const FunctorWrapperBase& rhs) const
            {
                if (Size != rhs.Size) return Size < rhs.Size;
                return memcmp((void*)this, (void*)&rhs, Size) < 0;
            }
        };

        template<typename F>
        struct FunctorWrapper : public FunctorWrapperBase
        {
            F LiveFunctor;

            FunctorWrapper(const F& func) : FunctorWrapperBase(sizeof(FunctorWrapper), (uint8_t*)&LiveFunctor), LiveFunctor(func) { }
        };
    }

    /**
     * \brief Given a callable type, returns the delegate-compatible call signature of that type.
     * 
     * The call signature removes class scope, calling convention, and pointer/reference characteristics.
     */
    template<typename T> using signature_of = typename Details::SignatureTraits<T>::Sig;
    template<typename A, typename B> constexpr bool signature_match = std::is_same_v<signature_of<A>, signature_of<B>>;
    template<typename T> constexpr bool is_signature = Details::IsSignature<T>::value;
}
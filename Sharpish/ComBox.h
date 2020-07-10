#pragma once

#include "Object.h"


namespace CS
{
	template<class T>
	class ComBox : public ComClass<ComBox<T>>
	{
		const T _value;

	public:
		typedef T BoxedType;

		ComBox(T_IN(T) value) : _value(value) { }

		PROPERTY_READONLY(const T&, Value);
		const T& GetValue() { return _value; }
	};

	typedef com_ptr<IObject> BoxedValue;
}

IS_GENERIC_REFTYPE(1, ::CS::ComBox, "9AE6A991-912C-4B95-9D8E-5ECE8997D8D8")

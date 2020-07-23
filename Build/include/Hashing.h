#pragma once 

///////////////////////////////////
/// Default hash function provider. 
///////////////////////////////////

template<class T, class U> T HashAppend(T& hash, U appendValue) { return hash = (T)(((((UINT64)hash) << 5) | (((UINT64)hash) >> (sizeof(T) * 8 - 5))) ^ (UINT64)appendValue); }

#define DECLARE_HASHABLE(type) namespace std { template<> struct hash<type> : public hash_EveStandard<type> { }; }
#define DECLARE_HASHABLE_FUNC(type, argname, f) namespace std { template<> struct hash<type> : public hash_EveStandard<type> \
{ value_type operator()(argument_type const& argname) const f }; }

namespace std
{
	template<class T>
	struct hash_EveStandard
	{
		typedef T argument_type;
		typedef std::size_t value_type;

		value_type operator()(argument_type const& d) const
		{
			static const int x = sizeof(T) / sizeof(value_type);
			static const int shift = 5;
			static const int rsh = (8 * sizeof(value_type) - shift);
			static const value_type mask = (value_type)((1 << shift) - 1);
			const value_type* ptr = (value_type*)&d;
			value_type h = 0;

			for (int i = 0; i < x; i++)
				h = ((h << shift) | ((h >> rsh) & mask)) ^ ptr[i];

			return h;
		}
	};

	template<>
	struct hash<UUID>
	{
		size_t operator()(const UUID& id) const
		{
			auto arr = (const size_t*)&id;

			if (sizeof(size_t) == 8)
				return arr[0] ^ arr[1];
			else
				return arr[0] ^ arr[1] ^ arr[2] ^ arr[3];
		}
	};
}

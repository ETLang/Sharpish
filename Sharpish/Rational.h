#pragma once

namespace CS
{
	struct Rational
	{
		int32_t Num;
		uint32_t Den;

		Rational() { }
		Rational(const Rational& copy) : Num(copy.Num), Den(copy.Den) { }
		Rational(int32_t x) : Num(x), Den(1) { }
		Rational(int32_t num, uint32_t den) : Num(num), Den(den) { }
		
		template<int Precision = 8>
		static Rational FromFloat(float value)
		{
			int factor = CompileTimePow<Precision>::Of10;
			return Rational((int)(value * factor), factor);
		}

		Rational Simplify() const;

		inline Rational operator +(const Rational& rhs) const
		{
			if (Den == rhs.Den) return Rational(Num + rhs.Num, Den);
			return Rational(Num * rhs.Den + rhs.Num * Den, Num * Den).Simplify();
		}

		inline Rational operator -(const Rational& rhs) const
		{
			if (Den == rhs.Den) return Rational(Num - rhs.Num, Den);
			return Rational(Num * rhs.Den - rhs.Num * Den, Num * Den).Simplify();
		}

		inline Rational operator *(const Rational& rhs) const
		{
			return Rational(Num * rhs.Num, Den * rhs.Den);
		}

		inline Rational operator /(const Rational& rhs) const
		{
			return Rational(Num * rhs.Den, Den * rhs.Num);
		}

		inline bool operator ==(const Rational& rhs) const
		{
			auto a = Simplify();
			auto b = rhs.Simplify();
			return a.Num == b.Num && a.Den == b.Den;
		}

		inline bool operator !=(const Rational& rhs) const
		{
			return !(*this == rhs);
		}

		inline bool operator <(const Rational& rhs) const
		{
			return Num * rhs.Den < Den * rhs.Num;
		}

		inline bool operator >(const Rational& rhs) const
		{
			return Num * rhs.Den > Den * rhs.Num;
		}

		inline bool operator <=(const Rational& rhs) const
		{
			return Num * rhs.Den <= Den * rhs.Num;
		}

		inline bool operator >=(const Rational& rhs) const
		{
			return Num * rhs.Den >= Den * rhs.Num;
		}

		inline operator bool() const { return Num != 0; }

	private:
		template<int Power> struct CompileTimePow 
		{ static const int Of10 = 10 * CompileTimePow<Power - 1>::Of10; };

		template<> struct CompileTimePow<0>
		{ static const int Of10 = 1; };
	};
}

IS_VALUETYPE(::CS::Rational, "60B9DC9A-BBEE-469B-8027-F7EE76B129C4");
DECLARE_HASHABLE(::CS::Rational);

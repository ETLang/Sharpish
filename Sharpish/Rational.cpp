#include "Sharpish.h"
#include "Rational.h"

using namespace CS;

Rational Rational::Simplify() const 
{
	int gcd = Help::Math::GCD(abs(Num), Den); 
	return Rational(Num / gcd, Den / gcd); 
}
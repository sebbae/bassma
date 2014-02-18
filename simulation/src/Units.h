/*
 * Units.h
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#ifndef UNITS_H_
#define UNITS_H_

namespace bassma {

template<int M, int K, int S>
struct Unit {
	enum { m = M, kg = K, s = S };
};

template<typename Unit>
struct Value {
	double val;
	explicit Value(double d) : val(d) {}
};

using Speed = Value<Unit<1,0,-1>>;
using Acceleration = Value<Unit<1,0,-2>>;

struct Angle {
	double val;
	explicit Angle(double d) : val(d) {}
};

constexpr Value<Speed> operator"" _ms(double d) {
	return Value<Speed>(d);
}

constexpr Angle operator"" _deg(double d) {
	return Angle(d);
}

} /* namespace bassma */

#endif /* UNITS_H_ */

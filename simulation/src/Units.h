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
	explicit constexpr Value(double d) : val(d) {}
	const Value operator+(const Value& rhs) {
		return Value(val + rhs.val);
	}
	const Value operator-(const Value& rhs) {
		return Value(val -rhs.val);
	}
	const Value operator-() {
		return Value(-val);
	}
	Value& operator+=(const Value& rhs) {
		val += rhs.val;
		return *this;
	}
	Value& operator-=(const Value& rhs) {
		val -= rhs.val;
		return *this;
	}
	bool operator<(const Value& rhs) {
		return val < rhs.val;
	}
	bool operator>(const Value& rhs) {
		return val > rhs.val;
	}
	bool operator==(const Value& rhs) {
		return val == rhs.val;
	}
};

using Time = Value<Unit<0,0,1>>;
using Distance = Value<Unit<1,0,0>>;
using Speed = Value<Unit<1,0,-1>>;
using Acceleration = Value<Unit<1,0,-2>>;

struct Angle {
	double val;
	explicit constexpr Angle(double d) : val(d) {}
	const Angle operator+(const Angle& rhs) {
		return Angle(val + rhs.val);
	}
	const Angle operator-(const Angle& rhs) {
		return Angle(val -rhs.val);
	}
	const Angle operator-() {
		return Angle(-val);
	}
	Angle& operator+=(const Angle& rhs) {
		val += rhs.val;
		return *this;
	}
};

constexpr Time operator"" _s(long double d) {
	return Time(d);
}

constexpr Speed operator"" _ms(long double d) {
	return Speed(d);
}

constexpr Acceleration operator"" _ms2(long double d) {
	return Acceleration(d);
}

constexpr Angle operator"" _deg(long double d) {
	return ((d > 360.0) || (d < -360.0)) ? Angle(d / 360.0) : Angle(d);
}

inline Speed operator*(const double& d, const Speed& s) {
	return Speed(d * s.val);
}

inline Speed operator*(const Acceleration& a, const Time& t) {
	return Speed(a.val * t.val);
}

inline Distance operator*(const Speed& s, const Time& t) {
	return Distance(s.val * t.val);
}

inline Distance operator+(const Distance& lhs, const Distance& rhs) {
	return Distance(lhs.val + rhs.val) ;
}

inline Acceleration operator*(const double& d, const Acceleration& v) {
	return Acceleration(d * v.val);
}

} /* namespace bassma */

#endif /* UNITS_H_ */

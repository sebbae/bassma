/*
 * CruiseControl.h
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#ifndef CRUISECONTROL_H_
#define CRUISECONTROL_H_

#include "Units.h"

namespace bassma {

class CruiseControl {
public:
	CruiseControl() {};
	virtual ~CruiseControl() = 0;

	CruiseControl(const CruiseControl& ctrl) = delete;
	CruiseControl& operator=(const CruiseControl& ctrl) = delete;

	virtual Speed getSpeed() = 0;
	virtual void setSpeed(Speed speed) = 0;
	virtual void turn(Angle angle) = 0;
};

} /* namespace bassma */

#endif /* CRUISECONTROL_H_ */

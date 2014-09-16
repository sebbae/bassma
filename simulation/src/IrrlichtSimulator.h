/*
 * IrrlichtVideoSource.h
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#ifndef IRRLICHTVIDEOSOURCE_H_
#define IRRLICHTVIDEOSOURCE_H_

#include "VideoSource.h"
#include "CruiseControl.h"

#include <memory>

namespace bassma {

class IrrlichtSimulatorImpl;

class IrrlichtSimulator: public VideoSource, public CruiseControl {
public:
	IrrlichtSimulator(void* windowId = nullptr, int width = 640, int height = 480);
	virtual ~IrrlichtSimulator();

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(AngularVelocity velocity);
private:
	std::shared_ptr<IrrlichtSimulatorImpl> impl;
};

} /* namespace bassma */

#endif /* IRRLICHTVIDEOSOURCE_H_ */

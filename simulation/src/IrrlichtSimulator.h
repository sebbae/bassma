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

#include <irrlicht.h>
#include <memory>

namespace bassma {

class IrrlichtRunner;

class IrrlichtSimulator: public VideoSource, public CruiseControl {
public:
	IrrlichtSimulator(irr::IrrlichtDevice* device);
	virtual ~IrrlichtSimulator();

	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(Angle angle);
private:
	std::shared_ptr<IrrlichtRunner> runner;
};

} /* namespace bassma */

#endif /* IRRLICHTVIDEOSOURCE_H_ */

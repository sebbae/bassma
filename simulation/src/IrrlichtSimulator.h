/*
 * IrrlichtVideoSource.h
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#ifndef IRRLICHTVIDEOSOURCE_H_
#define IRRLICHTVIDEOSOURCE_H_

#include "VideoSource.h"

#include <irrlicht.h>
#include <memory>

namespace bassma {

class IrrlichtRunner;

class IrrlichtSimulator: public bassma::VideoSource {
public:
	IrrlichtSimulator();
	virtual ~IrrlichtSimulator();

	virtual cv::Mat captureFrame();
private:
	std::shared_ptr<IrrlichtRunner> runner;
};

} /* namespace bassma */

#endif /* IRRLICHTVIDEOSOURCE_H_ */

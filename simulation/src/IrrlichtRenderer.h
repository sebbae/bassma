/*
 * IrrlichtRenderer.h
 *
 *  Created on: 06.07.2014
 *      Author: sebastian
 */

#ifndef IRRLICHTRENDERER_H_
#define IRRLICHTRENDERER_H_

#include "VideoSource.h"
#include "CruiseControl.h"

#include <irrlicht.h>
#include <memory>

namespace bassma {

class IrrlichtRendererImpl;

class IrrlichtRenderer: public VideoSource, public CruiseControl {
public:
	IrrlichtRenderer(void* windowId = nullptr, int width = 640, int height = 480);
	virtual ~IrrlichtRenderer();

	bool update(int width, int height);

	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(Angle angle);
private:
	std::unique_ptr<IrrlichtRendererImpl> impl;
};

} /* namespace bassma */

#endif /* IRRLICHTRENDERER_H_ */

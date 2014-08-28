/*
 * OpenCVWebcam.h
 *
 *  Created on: 24.07.2014
 *      Author: sebastian
 */

#ifndef OPENCVWEBCAM_H_
#define OPENCVWEBCAM_H_

#include <VideoSource.h>

#include <memory>

namespace bassma {

class OpenCVWebcamImpl;

class OpenCVWebcam: public VideoSource {
public:
	OpenCVWebcam();
	virtual ~OpenCVWebcam();

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

private:
	std::unique_ptr<OpenCVWebcamImpl> impl;
};

} /* namespace bassma */

#endif /* OPENCVWEBCAM_H_ */

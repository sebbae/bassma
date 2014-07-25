/*
 * OpenCVWebcam.cpp
 *
 *  Created on: 24.07.2014
 *      Author: sebastian
 */

#include <OpenCVWebcam.h>

#include "opencv2/highgui/highgui.hpp"

namespace bassma {

class OpenCVWebcamImpl {
public:
	OpenCVWebcamImpl();
	~OpenCVWebcamImpl();
	cv::Mat captureFrame();
private:
	cv::VideoCapture capture;
};

bassma::OpenCVWebcamImpl::OpenCVWebcamImpl() {
	capture.open(-1);
	if (!capture.isOpened()) {
		throw "Error opening video capture";
	}
}

bassma::OpenCVWebcamImpl::~OpenCVWebcamImpl() {
	if (capture.isOpened()) {
		capture.release();
	}
}

cv::Mat bassma::OpenCVWebcamImpl::captureFrame() {
	cv::Mat frame;
	capture.read(frame);
	return frame;
}

OpenCVWebcam::OpenCVWebcam() : impl(std::unique_ptr<OpenCVWebcamImpl>(new OpenCVWebcamImpl())) {
}

OpenCVWebcam::~OpenCVWebcam() {
}

cv::Mat OpenCVWebcam::captureFrame() {
	return impl->captureFrame();
}

} /* namespace bassma */

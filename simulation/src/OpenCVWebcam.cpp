/*
 * OpenCVWebcam.cpp
 *
 *  Created on: 24.07.2014
 *      Author: sebastian
 */

#include <OpenCVWebcam.h>

#include "opencv2/highgui/highgui.hpp"
#include <iostream>

namespace bassma {

class OpenCVWebcamImpl {
public:
	OpenCVWebcamImpl();
	~OpenCVWebcamImpl();
	void resize(int width, int height);
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
		std::cout << "release capture" << std::endl;
		capture.release();
	}
}

cv::Mat bassma::OpenCVWebcamImpl::captureFrame() {
	cv::Mat frame;
	capture.read(frame);
	return frame;
}

void bassma::OpenCVWebcamImpl::resize(int width, int height) {
	capture.set(CV_CAP_PROP_FRAME_WIDTH, width);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

OpenCVWebcam::OpenCVWebcam() : impl(std::unique_ptr<OpenCVWebcamImpl>(new OpenCVWebcamImpl())) {
}

OpenCVWebcam::~OpenCVWebcam() {
}

void OpenCVWebcam::resize(int width, int height) {
	impl->resize(width, height);
}

cv::Mat OpenCVWebcam::captureFrame() {
	return impl->captureFrame();
}

} /* namespace bassma */

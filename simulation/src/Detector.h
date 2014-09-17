/*
 * Detector.h
 *
 *  Created on: 17.09.2014
 *      Author: sebastian
 */

#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>

namespace bassma {

class Body {
public:
	Body() {};
	virtual ~Body() {};
};

class Detector {
public:
	Detector() {};
	virtual ~Detector() {};

	Detector(const Detector& d) = delete;
	Detector& operator=(const Detector& d) = delete;

	virtual std::vector<std::unique_ptr<Body>> detect(cv::Mat& frame, cv::Mat& detectFrame, bool markOnFrame = true) = 0;
};

} /* namespace bassma */

#endif /* DETECTOR_H_ */

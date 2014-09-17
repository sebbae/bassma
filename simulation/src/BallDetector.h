/*
 * BallDetector.h
 *
 *  Created on: 17.09.2014
 *      Author: sebastian
 */

#ifndef BALLDETECTOR_H_
#define BALLDETECTOR_H_

#include "Detector.h"

namespace bassma {

class Ball : public Body {
public:
	Ball(int x, int y, int radius) : x(x), y(y), radius(radius) {}
	int x;
	int y;
	int radius;
};

class BallDetector: public Detector {
public:
	BallDetector();
	virtual ~BallDetector();

	void setBlur(int blur);
	void setDP(double dp);

	virtual std::vector<std::unique_ptr<Body>> detect(cv::Mat& frame, cv::Mat& detectFrame, bool markOnFrame = true);
private:
	int blur;
	double dp;
};

} /* namespace bassma */

#endif /* BALLDETECTOR_H_ */

/*
 * BallDetector.cpp
 *
 *  Created on: 17.09.2014
 *      Author: sebastian
 */

#include "BallDetector.h"

namespace bassma {

BallDetector::BallDetector() : blur(3), dp(1) {
}

BallDetector::~BallDetector() {
}

void BallDetector::setBlur(int blur) {
	this->blur = blur;
}

void BallDetector::setDP(double dp) {
	this->dp = dp;
}

std::vector<std::unique_ptr<Body> > BallDetector::detect(cv::Mat& frame,
		cv::Mat& detectFrame,
		bool markOnFrame) {
	std::vector<std::unique_ptr<Body>> balls;

	cv::Mat bw;
	cv::cvtColor(frame, bw, CV_BGR2GRAY);

	/// Reduce the noise so we avoid false circle detection
	cv::GaussianBlur( bw, bw, cv::Size(blur, blur), 2, 2 );

	/// Apply the Hough Transform to find the circles
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(bw, circles, CV_HOUGH_GRADIENT, dp, 30, 210, 110, 10, 0);
	detectFrame = bw;

	/// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++) {
		int x = cvRound(circles[i][0]);
		int y = cvRound(circles[i][1]);
		int radius = cvRound(circles[i][2]);
		balls.push_back(std::unique_ptr<Body>(new Ball(x, y, radius)));
		cv::Point center(x, y);
		cv::circle(detectFrame, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		cv::circle(detectFrame, center, radius, cv::Scalar(0, 0, 255), 2, 8, 0);
		if (markOnFrame) {
			cv::circle(frame, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), 2, 8, 0);
		}
	}

	return balls;
}

} /* namespace bassma */

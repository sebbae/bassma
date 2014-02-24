/*
 * simulator.cpp
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#include "VideoSource.h"
#include "IrrlichtSimulator.h"

#include <opencv2/opencv.hpp>
#include <memory>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
	using namespace bassma;

	std::unique_ptr<VideoSource> video(new IrrlichtSimulator());

	cv::Mat frame = video->captureFrame();
	cv::namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
	cv::imshow("Display Image", frame);
	cv::waitKey(50);
	std::string s;
	std::getline(std::cin, s);
}

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
#include <thread>
#include <chrono>

cv::Mat canny(const cv::Mat& frame) {
	cv::Mat bw;
	cv::cvtColor(frame, bw, CV_BGR2GRAY);
	cv::blur(bw, bw, cv::Size(3, 3));
	cv::Canny(bw, bw, 100, 100, 3);
	return bw;
}

cv::Mat detectCircles(cv::Mat& frame) {
	cv::Mat bw;
	cv::cvtColor(frame, bw, CV_BGR2GRAY);

	/// Reduce the noise so we avoid false circle detection
	cv::GaussianBlur( bw, bw, cv::Size(9, 9), 2, 2 );

	/// Apply the Hough Transform to find the circles
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(bw, circles, CV_HOUGH_GRADIENT, 1, bw.rows / 8, 500, 400, 0, 0);
	std::cout << "Circles found: " << circles.size() << std::endl;

	/// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++) {
		cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		cv::circle(frame, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
		cv::circle(frame, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
	}
	return bw;
}

cv::Mat detectColor(cv::Mat frame) {
	using namespace cv;

    // Convert the image into an HSV image
	cv::Mat hsv;
	cv::Mat threshold;
	cv::cvtColor(frame, hsv, CV_BGR2HSV);

	cv::inRange(hsv, Scalar(40, 75, 200),Scalar(57, 255, 218),threshold);

    return hsv;
}

int main(int argc, char* argv[]) {
	using namespace bassma;

	std::unique_ptr<IrrlichtSimulator> sim(new IrrlichtSimulator());
	//sim->setSpeed(30.0_ms);

	while (true) {
		cv::Mat frame = sim->captureFrame();
		cv::Mat out = frame;

		//out = detectColor(frame);
		//out = canny(frame);
	    //out = detectCircles(frame);

		cv::namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
		cv::imshow("Display Image", out);
		cv::waitKey(50);
		std::chrono::milliseconds duration(50);
		std::this_thread::sleep_for(duration);
	}

//	std::string s;
//	std::getline(std::cin, s);
}

/*
#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
 Mat image, src;
 Mat HSV;
 char window_name[20]="Pixel Value Demo";

static void onMouse( int event, int x, int y, int f, void* ){
 image=src.clone();
 Vec3b pix=HSV.at<Vec3b>(y,x);
 int H=pix.val[0];
 int S=pix.val[1];
 int V=pix.val[2];


 char name[30];
    sprintf(name,"H=%d",H);
    putText(image,name, Point(10,130) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"S=%d",S);
    putText(image,name, Point(10,170) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"V=%d",V);
    putText(image,name, Point(10,210) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"X=%d",x);
    putText(image,name, Point(10,300) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );

    sprintf(name,"Y=%d",y);
    putText(image,name, Point(10,340) , FONT_HERSHEY_SIMPLEX, .7, Scalar(0,255,0), 2,8,false );
 imshow( window_name, image );
}



int main( int argc, char** argv )
{
  namedWindow( window_name, CV_WINDOW_AUTOSIZE );

  src = imread( "image.jpg");
  imshow( window_name, src );
  cvtColor(src,HSV,CV_BGR2HSV);
  setMouseCallback( window_name, onMouse, 0 );

  waitKey(0);

  return 0;
}
 */

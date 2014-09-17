/*
 * QBallDetector.cpp
 *
 *  Created on: 17.09.2014
 *      Author: sebastian
 */

#include "QBallDetector.h"
#include "ui_QBallDetector.h"
#include "../BallDetector.h"
#include <iostream>

namespace bassma {

QBallDetector::QBallDetector(QWidget* parent) :
		QWidget(parent), ui(new bassma::Ui::QBallDetector), impl(
				new BallDetector()) {
	ui->setupUi(this);
	setBlur(ui->blurSlider->value());
	setDP(ui->dpSlider->value());
}

QBallDetector::~QBallDetector() {
}

std::vector<std::unique_ptr<Body> > QBallDetector::detect(cv::Mat& frame,
		cv::Mat& detectFrame, bool markOnFrame) {
	std::vector<std::unique_ptr<Body> > balls = impl->detect(frame, detectFrame, markOnFrame);
	ui->image->showImage(detectFrame);
	return balls;
}

void QBallDetector::setBlur(int blur) {
	std::cout << "setBlur"  << blur << std::endl;
	impl->setBlur(((blur - 1) * 2) + 1);
}

void QBallDetector::setDP(int dp) {
	std::cout << "setDP"  << dp << std::endl;
	impl->setDP((double) dp / (double) 100);
}

} /* namespace bassma */

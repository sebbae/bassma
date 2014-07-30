/*
 * OpenCVWebcamWidget.cpp
 *
 *  Created on: 25.07.2014
 *      Author: sebastian
 */


#include <QOpenCVWebcamWidget.h>

#include <QResizeEvent>
#include <iostream>

namespace bassma {

QOpenCVWebcamWidget::QOpenCVWebcamWidget(QWidget* parent) : QOpenCVWidget(parent) {
	connect(this, SIGNAL(updateQuery()), this, SLOT(update()));
	startTimer(0);
}

QOpenCVWebcamWidget::~QOpenCVWebcamWidget() {
}

cv::Mat QOpenCVWebcamWidget::captureFrame() {
	return webcam.captureFrame();
}

void QOpenCVWebcamWidget::timerEvent(QTimerEvent* event) {
	emit updateQuery();
	event->accept();
}

void QOpenCVWebcamWidget::update() {
	showImage(webcam.captureFrame());
}

} /* namespace bassma */

/*
 * QSimulator.cpp
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#include "QSimulator.h"
#include "ui_QSimulator.h"

namespace bassma {

QSimulator::QSimulator(VideoSource* video, QWidget *parent) :
		video(video), QMainWindow(parent), ui(new bassma::Ui::QSimulator) {
	ui->setupUi(this);
}

QSimulator::~QSimulator() {
}

void QSimulator::on_actionStart_triggered() {
	startTimer(0);
}

void QSimulator::timerEvent(QTimerEvent *event) {
	cv::Mat image = video->captureFrame();
	// Do what you want with the image :-)
	// Show the image
	ui->openCVviewer->showImage(image);
}

} /* namespace bassma */

/*
 * QSimulation.cpp
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#include <iostream>

#include "QSimulation.h"
#include "ui_QSimulation.h"
#include "QOpenCVWebcamWidget.h"
#include "QIrrlichtWidget.h"

namespace bassma {

QSimulation::QSimulation(QWidget *parent) :
		QWidget(parent), ui(new bassma::Ui::QSimulation) {
	ui->setupUi(this);
}

QSimulation::~QSimulation() {
}

void QSimulation::resizeEvent(QResizeEvent* event) {
	std::cout << "QSimulation resize event " << event << std::endl;
	QWidget::resizeEvent(event);
}


void QSimulation::resize(int width, int height) {
	ui->video->resize(width, height);
}

cv::Mat QSimulation::captureFrame() {
	return ui->video->captureFrame();
}

void QSimulation::setSpeed(int speed) {
	std::cout << "setSpeed(" << speed << ")" << std::endl;
}


} /* namespace bassma */

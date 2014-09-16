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
    ui->video->installEventFilter(this);
    ui->speedSlider->installEventFilter(this);
    ui->turnSlider->installEventFilter(this);
}

QSimulation::~QSimulation() {
}

void QSimulation::resizeEvent(QResizeEvent* event) {
	std::cout << "QSimulation resize event " << event << std::endl;
	QWidget::resizeEvent(event);
}


bool QSimulation::eventFilter(QObject* object, QEvent* event) {
	if (event->type() != QEvent::KeyPress) {
		return false;
	}
	QKeyEvent* keyEvent = static_cast<QKeyEvent *>(event);
	if (keyEvent->key() == Qt::Key_Left) {
		ui->turnSlider->setValue(ui->turnSlider->value() - 1);
	} else if (keyEvent->key() == Qt::Key_Right) {
		ui->turnSlider->setValue(ui->turnSlider->value() + 1);
	} else if (keyEvent->key() == Qt::Key_Up) {
		ui->speedSlider->setValue(ui->speedSlider->value() + 1);
	} else if (keyEvent->key() == Qt::Key_Down) {
		ui->speedSlider->setValue(ui->speedSlider->value() -1);
	} else {
		return false;
	}
	return true;
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

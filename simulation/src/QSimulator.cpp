/*
 * QSimulator.cpp
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#include <iostream>

#include "QSimulator.h"
#include "ui_QSimulator.h"

namespace bassma {

QSimulator::QSimulator(QWidget *parent) :
		QMainWindow(parent), ui(new bassma::Ui::QSimulator) {
	ui->setupUi(this);
	ui->irrlichtViewer->init();
	renderer = std::unique_ptr<IrrlichtRenderer>(new IrrlichtRenderer(ui->irrlichtViewer->getIrrlichtDevice()));
}

QSimulator::~QSimulator() {
}

void QSimulator::on_actionStart_triggered() {
	startTimer(10);
}

void QSimulator::timerEvent(QTimerEvent *event) {
	std::cout << "Updating frame" << std::endl;
	renderer->update(ui->irrlichtViewer->size().width(), ui->irrlichtViewer->size().height());
	cv::Mat image = renderer->captureFrame();
	// Do what you want with the image :-)
	// Show the image
	ui->openCVviewer->showImage(image);
	std::cout << "Updated frame" << std::endl;
}

} /* namespace bassma */

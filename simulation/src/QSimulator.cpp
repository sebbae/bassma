/*
 * QSimulator.cpp
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#include <iostream>

#include "QSimulator.h"
#include "ui_QSimulator.h"
#include "QOpenCVWebcamWidget.h"
#include "QIrrlichtWidget.h"

namespace bassma {

QSimulator::QSimulator(QWidget *parent) :
		QMainWindow(parent), ui(new bassma::Ui::QSimulator) {
	ui->setupUi(this);
	//ui->irrlichtViewer->init();
	//std::cout << " QSimulator()" << " with winId " << reinterpret_cast<void*>(this->winId()) << std::endl;
}

QSimulator::~QSimulator() {
}

void QSimulator::on_actionVideoWebcam_triggered() {
	std::cout << "on_actionVideoWebcam_triggered" << std::endl;
	ui->videoLayout->removeWidget(ui->source);
	ui->source->close();
	videoSource = new QOpenCVWebcamWidget(ui->centralWidget);
	ui->source = dynamic_cast<QWidget*>(videoSource);
	ui->videoLayout->addWidget(ui->source, 0,0,1,1);
	ui->videoLayout->update();
	startTimer(10);
}

void QSimulator::on_actionVideoIrrlicht_triggered() {
	std::cout << "on_actionVideoIrrlicht_triggered" << std::endl;
	ui->videoLayout->removeWidget(ui->source);
	ui->source->close();
	videoSource = new QIrrlichtWidget(ui->centralWidget);
	ui->source = dynamic_cast<QWidget*>(videoSource);
	ui->videoLayout->addWidget(ui->source, 0,0,1,1);
	ui->videoLayout->update();
	startTimer(100);
}

void QSimulator::timerEvent(QTimerEvent* event) {
	std::cout << "Updating frame" << std::endl;
	//renderer->update(ui->irrlichtViewer->size().width(), ui->irrlichtViewer->size().height());
	cv::Mat image = videoSource->captureFrame();
	// Do what you want with the image :-)
	// Show the image
	if (!image.empty()) {
		ui->processed->showImage(image);
	}
	std::cout << "Updated frame" << std::endl;
	event->accept();
}

} /* namespace bassma */

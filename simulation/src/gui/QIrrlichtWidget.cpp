/*
 * QIrrlichtWidget.cpp
 *
 *  Created on: 14.04.2014
 *      Author: sebastian
 */

#include "QIrrlichtWidget.h"
#include <QtCore/QDebug>
#include <opencv2/core/core.hpp>

#include <iostream>

namespace bassma {

QIrrlichtWidget::QIrrlichtWidget(QWidget* parent) :
		QWidget(parent) {
	// Indicates that the widget wants to draw directly onto the screen. (From documentation : http://doc.qt.nokia.com/latest/qt.html)
	// Essential to have this or there will be nothing displayed
	setAttribute(Qt::WA_PaintOnScreen);
	// Indicates that the widget paints all its pixels when it receives a paint event.
	// Thus, it is not required for operations like updating, resizing, scrolling and focus changes to erase the widget before generating paint events.
	// Not sure this is required for the program to run properly, but it is here just incase.
	setAttribute(Qt::WA_OpaquePaintEvent);
	// Widget accepts focus by both tabbing and clicking
	//setFocusPolicy(Qt::StrongFocus);
	//setAttribute(Qt::WA_NoBackground);
	//setAttribute(Qt::WA_NoSystemBackground);
	// Not sure if this is necessary, but it was in the code I am basing this solution off of
	setAutoFillBackground(false);
	std::cout << " QIrrlichtWidget()" << " with winId " << reinterpret_cast<void*>(this->winId()) << std::endl;
	void* winId = reinterpret_cast<void*>(this->winId());
	QSize size = this->maximumSize();
	simulator = std::unique_ptr<IrrlichtSimulator>(new IrrlichtSimulator(winId, size.width(), size.height()));
	//renderer->setSpeed(1.0_ms);
	// Connect the update signal (updateQuery) to the update slot (update)
	connect(this, SIGNAL(updateQuery()), this, SLOT(update()));

	// Start a timer. A timer with setting 0 will update as often as possible.
	startTimer(0);
}

QIrrlichtWidget::~QIrrlichtWidget() {
}

void QIrrlichtWidget::paintEvent(QPaintEvent* event) {
	//emit updateQuery();
	update();
}

void QIrrlichtWidget::timerEvent(QTimerEvent* event) {
	// Emit the render signal each time the timer goes off
	emit updateQuery();
	event->accept();
}

void QIrrlichtWidget::resizeEvent(QResizeEvent* event) {
	std::cout << "QIrrlichtWidget resize event " << event << std::endl;
	update();
	QWidget::resizeEvent(event);
}

void QIrrlichtWidget::update() {
	QSize size = this->size();
	simulator->resize(size.width(), size.height());
}

void QIrrlichtWidget::resize(int width, int height) {
	simulator->resize(width, height);
}

cv::Mat QIrrlichtWidget::captureFrame() {
	return simulator->captureFrame();
}

Speed QIrrlichtWidget::getSpeed() {
	return simulator->getSpeed();
}

void QIrrlichtWidget::setSpeed(Speed speed) {
	simulator->setSpeed(Speed(speed.val * 0.1));
}

void QIrrlichtWidget::turn(AngularVelocity velocity) {
	simulator->turn(velocity);
}

void QIrrlichtWidget::turn(int velocity) {
	turn(AngularVelocity(velocity));
}

void QIrrlichtWidget::setSpeed(int speed) {
	setSpeed(Speed(speed * 0.1));
}

} /* namespace bassma */

/*
 * QIrrlichtWidget.h
 *
 *  Created on: 14.04.2014
 *      Author: sebastian
 */

#ifndef QIRRLICHTWIDGET_H_
#define QIRRLICHTWIDGET_H_

#include <QWidget>
#include <QResizeEvent>
#include <memory>
#include <irrlicht.h>

#include "../VideoSource.h"
#include "../CruiseControl.h"
#include "../IrrlichtSimulator.h"

namespace bassma {

class QIrrlichtWidget: public QWidget, public VideoSource, public CruiseControl {
Q_OBJECT

public:
	explicit QIrrlichtWidget(QWidget* parent = 0);
	~QIrrlichtWidget();

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(AngularVelocity velocity);

signals:
	// Signal that its time to update the frame
	void updateQuery();

public slots:
	// Function called in response to updateIrrlichtQuery. Renders the scene in the widget
	void update();
	void setSpeed(int speed);
	void turn(int velocity);

protected:
	virtual void paintEvent(QPaintEvent* event);
	virtual void timerEvent(QTimerEvent* event);
	virtual void resizeEvent(QResizeEvent* event);

	std::unique_ptr<IrrlichtSimulator> simulator;

};

} /* namespace bassma */

#endif /* QIRRLICHTWIDGET_H_ */

/*
 * OpenCVWebcamWidget.h
 *
 *  Created on: 25.07.2014
 *      Author: sebastian
 */

#ifndef OPENCVWEBCAMWIDGET_H_
#define OPENCVWEBCAMWIDGET_H_

#include "../VideoSource.h"
#include "../OpenCVWebcam.h"
#include "QOpenCVWidget.h"

namespace bassma {

class QOpenCVWebcamWidget: public QOpenCVWidget, public VideoSource {
Q_OBJECT

public:
	explicit QOpenCVWebcamWidget(QWidget* parent = 0);
	virtual ~QOpenCVWebcamWidget();

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

signals:
	// Signal that its time to update the frame
	void updateQuery();

public slots:
	// Function called in response to updateIrrlichtQuery. Renders the scene in the widget
	void update();

protected:
	virtual void timerEvent(QTimerEvent* event);

private:
	OpenCVWebcam webcam;
};

} /* namespace bassma */

#endif /* OPENCVWEBCAMWIDGET_H_ */

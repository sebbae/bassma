/*
 * QOpenCVWidget.h
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#ifndef QOPENCVWIDGET_H_
#define QOPENCVWIDGET_H_

#include <QGLWidget>
#include <opencv2/core/core.hpp>

namespace bassma {

class QOpenCVWidget: public QGLWidget {
Q_OBJECT
public:
	explicit QOpenCVWidget(QWidget* parent = 0);
	~QOpenCVWidget();

signals:
	void imageSizeChanged(int outW, int outH);

public slots:
	bool showImage(cv::Mat image);

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void updateScene();
	void renderImage();

private:
	bool mSceneChanged;

	QImage mRenderQtImg;
	cv::Mat mOrigImage;

	QColor mBgColor;

	// Resized Image height / width
	int mOutH;
	int mOutW;
	float mImgRatio;

	// top left x/y to render image in the center of widget
	int mPosX;
	int mPosY;
};

} /* namespace bassma */

#endif /* QOPENCVWIDGET_H_ */

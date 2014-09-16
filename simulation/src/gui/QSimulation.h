/*
 * QSimulation.h
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#ifndef QSIMULATION_H_
#define QSIMULATION_H_

#include <QWidget>
#include <memory>

#include "../VideoSource.h"

namespace bassma {

namespace Ui {
	class QSimulation;
}

class VideoSource;

class QSimulation: public QWidget, public VideoSource {
	Q_OBJECT

public:
	explicit QSimulation(QWidget* parent = 0);
	~QSimulation();

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

protected:
	virtual void resizeEvent(QResizeEvent* event);
	virtual bool eventFilter(QObject* object, QEvent* event);

private slots:
	void setSpeed(int speed);

private:
	std::unique_ptr<bassma::Ui::QSimulation> ui;
};

} /* namespace bassma */

#endif /* QSIMULATION_H_ */

/*
 * QSimulator.h
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#ifndef QSIMULATOR_H_
#define QSIMULATOR_H_

#include <QMainWindow>
#include "VideoSource.h"
#include <memory>

namespace bassma {

namespace Ui {
	class QSimulator;
}

class QSimulator: public QMainWindow {
	Q_OBJECT

public:
	explicit QSimulator(VideoSource* video, QWidget* parent = 0);
	~QSimulator();

private slots:
	void on_actionStart_triggered();

private:
	std::unique_ptr<bassma::Ui::QSimulator> ui;
	VideoSource* video;

protected:
	void timerEvent(QTimerEvent *event);
};

} /* namespace bassma */

#endif /* QSIMULATOR_H_ */

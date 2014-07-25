/*
 * QSimulator.h
 *
 *  Created on: 02.04.2014
 *      Author: sebastian
 */

#ifndef QSIMULATOR_H_
#define QSIMULATOR_H_

#include <QMainWindow>
#include <memory>

namespace bassma {

namespace Ui {
	class QSimulator;
}

class VideoSource;

class QSimulator: public QMainWindow {
	Q_OBJECT

public:
	explicit QSimulator(QWidget* parent = 0);
	~QSimulator();

private slots:
	void on_actionVideoWebcam_triggered();
	void on_actionVideoIrrlicht_triggered();

private:
	std::unique_ptr<bassma::Ui::QSimulator> ui;
	std::unique_ptr<VideoSource> videoSource;

protected:
	void timerEvent(QTimerEvent *event);
};

} /* namespace bassma */

#endif /* QSIMULATOR_H_ */

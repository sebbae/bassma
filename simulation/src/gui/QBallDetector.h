/*
 * QBallDetector.h
 *
 *  Created on: 17.09.2014
 *      Author: sebastian
 */

#ifndef QBALLDETECTOR_H_
#define QBALLDETECTOR_H_

#include <QWidget>
#include <QResizeEvent>
#include <memory>
#include "../BallDetector.h"

namespace bassma {

namespace Ui {
	class QBallDetector;
}

class QBallDetector: public QWidget, public BallDetector {
	Q_OBJECT
public:
	explicit QBallDetector(QWidget* parent = 0);
	virtual ~QBallDetector();

	virtual std::vector<std::unique_ptr<Body>> detect(cv::Mat& frame, cv::Mat& detectFrame, bool markOnFrame = true);

private slots:
	void setBlur(int blur);
	void setDP(int dp);

private:
	std::unique_ptr<bassma::Ui::QBallDetector> ui;
	std::unique_ptr<BallDetector> impl;
};

} /* namespace bassma */

#endif /* QBALLDETECTOR_H_ */

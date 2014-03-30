/*
 * qtexample.cpp
 *
 *  Created on: 30.03.2014
 *      Author: sebastian
 */

#include <QApplication>
#include "cqtopencvviewergl.h"

int main(int argv, char **args) {
	QApplication app(argv, args);

	CQtOpenCVViewerGl viewer;
	viewer.show();

	return app.exec();
}


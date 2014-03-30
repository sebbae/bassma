/*
 * qtexample.cpp
 *
 *  Created on: 30.03.2014
 *      Author: sebastian
 */

#include <QApplication>
#include <QTextEdit>

int main(int argv, char **args) {
	QApplication app(argv, args);

	QTextEdit textEdit;
	textEdit.show();

	return app.exec();
}


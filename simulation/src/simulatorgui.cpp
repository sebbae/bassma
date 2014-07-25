#include <QApplication>
#include <QDebug>
#include <memory>

#include "IrrlichtSimulator.h"
#include "QSimulator.h"
#include "QIrrlichtWidget.h"

int main(int argc, char *argv[]) {
	using namespace bassma;

	int res = -1;

	try {
		QApplication a(argc, argv);
		bassma::QSimulator w(nullptr);
		//bassma::QIrrlichtWidget w(nullptr);
		w.show();

		res = a.exec();
	}
//    catch(QException &e)
//    {
//        qCritical() << QString("Exception: %1").arg( e.what() );
//    }
	catch (...) {
		qCritical() << QString("Unhandled Exception");
	}

	return res;
}

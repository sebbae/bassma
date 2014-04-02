#include <QApplication>
#include <QDebug>
#include <memory>

#include "IrrlichtSimulator.h"
#include "QSimulator.h"

int main(int argc, char *argv[]) {
	using namespace bassma;
	std::unique_ptr<IrrlichtSimulator> sim(new IrrlichtSimulator());

	int res = -1;

	try {
		QApplication a(argc, argv);
		bassma::QSimulator w(sim.get());
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

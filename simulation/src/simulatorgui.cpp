#include <QApplication>
#include <QDebug>
#include <memory>

#include "IrrlichtSimulator.h"
#include "QSimulator.h"

int main(int argc, char *argv[]) {
	using namespace bassma;

	int width = 640;
	int height = 480;

	irr::IrrlichtDevice* device = irr::createDevice(irr::video::EDT_OPENGL,
			irr::core::dimension2d<irr::u32>(width, height));

	if (!device) {
		throw "Unable to create OpenGL device.";
	}

	std::unique_ptr<IrrlichtSimulator> sim(new IrrlichtSimulator(device));

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

	device->drop();

	return res;
}

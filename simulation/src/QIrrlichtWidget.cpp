/*
 * QIrrlichtWidget.cpp
 *
 *  Created on: 14.04.2014
 *      Author: sebastian
 */

#include <QIrrlichtWidget.h>
#include <QtCore/QDebug>

namespace bassma {

QIrrlichtWidget::QIrrlichtWidget(QWidget *parent) :
		QWidget(parent) {
	// Indicates that the widget wants to draw directly onto the screen. (From documentation : http://doc.qt.nokia.com/latest/qt.html)
	// Essential to have this or there will be nothing displayed
	setAttribute(Qt::WA_PaintOnScreen);
	// Indicates that the widget paints all its pixels when it receives a paint event.
	// Thus, it is not required for operations like updating, resizing, scrolling and focus changes to erase the widget before generating paint events.
	// Not sure this is required for the program to run properly, but it is here just incase.
	setAttribute(Qt::WA_OpaquePaintEvent);
	// Widget accepts focus by both tabbing and clicking
	setFocusPolicy(Qt::StrongFocus);
	// Not sure if this is necessary, but it was in the code I am basing this solution off of
	setAutoFillBackground(false);

	device = 0;
}

QIrrlichtWidget::~QIrrlichtWidget() {
	if (device != 0) {
		device->closeDevice();
		device->drop();
	}
}

// Create the Irrlicht device and connect the signals and slots
void QIrrlichtWidget::init() {
	// Make sure we can't create the device twice
	if (device != 0)
		return;

	// Set all the device creation parameters
	SIrrlichtCreationParameters params;
	params.AntiAlias = 0;
	params.Bits = 32;
	params.DeviceType = EIDT_X11;
	params.Doublebuffer = true;
	params.DriverType = EDT_OPENGL;
	params.EventReceiver = 0;
	params.Fullscreen = false;
	params.HighPrecisionFPU = false;
	params.IgnoreInput = false;
	params.LoggingLevel = ELL_INFORMATION;
	params.Stencilbuffer = true;
	params.Stereobuffer = false;
	params.Vsync = false;
	// Specify which window/widget to render to
	params.WindowId = reinterpret_cast<void*>(winId());
	params.WindowSize.Width = width();
	params.WindowSize.Height = height();
	params.WithAlphaChannel = false;
	params.ZBufferBits = 16;

	// Create the Irrlicht Device with the previously specified parameters
	device = createDeviceEx(params);

	if (device) {
		// Create a camera so we can view the scene
		camera = device->getSceneManager()->addCameraSceneNode(0,
				vector3df(0, 30, -40), vector3df(0, 5, 0));
	}

	// Connect the update signal (updateIrrlichtQuery) to the update slot (updateIrrlicht)
	connect(this, SIGNAL(updateIrrlichtQuery(IrrlichtDevice*)), this,
			SLOT(updateIrrlicht(IrrlichtDevice*)));

	// Start a timer. A timer with setting 0 will update as often as possible.
	startTimer(0);
}

IrrlichtDevice* QIrrlichtWidget::getIrrlichtDevice() {
	return device;
}

void QIrrlichtWidget::paintEvent(QPaintEvent* event) {
	if (device != 0) {
		emit updateIrrlichtQuery(device);
	}
}

void QIrrlichtWidget::timerEvent(QTimerEvent* event) {
	// Emit the render signal each time the timer goes off
	if (device != 0) {
		emit updateIrrlichtQuery(device);
	}

	event->accept();
}

void QIrrlichtWidget::resizeEvent(QResizeEvent* event) {
	if (device != 0) {
		dimension2d<u32> widgetSize;
		widgetSize.Width = event->size().width();
		widgetSize.Height = event->size().height();
		device->getVideoDriver()->OnResize(widgetSize);

		ICameraSceneNode *cam = device->getSceneManager()->getActiveCamera();
		if (cam != 0) {
			cam->setAspectRatio(
					(f32) widgetSize.Height / (f32) widgetSize.Width);
		}
	}

	QWidget::resizeEvent(event);
}

void QIrrlichtWidget::updateIrrlicht(irr::IrrlichtDevice* device) {
	if (device != 0) {
		device->getTimer()->tick();

		SColor color(255, 100, 100, 140);

		device->getVideoDriver()->beginScene(true, true, color);
		device->getSceneManager()->drawAll();
		device->getVideoDriver()->endScene();
	}

}

} /* namespace bassma */

/*
 * IrrlichtVideoSource.cpp
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#include "IrrlichtSimulator.h"
#include "IrrlichtRenderer.h"

#include <irrlicht.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace bassma {

irr::IrrlichtDevice* createNativeDevice(int width, int height) {
	using namespace irr;
	return createDevice(video::EDT_OPENGL, core::dimension2d<u32>(width, height));
}

irr::IrrlichtDevice* createGuestDevice(void* windowId, int width, int height) {
	using namespace irr;
	// Set all the device creation parameters
	SIrrlichtCreationParameters params;
	params.AntiAlias = 0;
	params.Bits = 32;
	params.DeviceType = EIDT_X11;
	params.Doublebuffer = true;
	params.DriverType = video::EDT_OPENGL;
	params.EventReceiver = 0;
	params.Fullscreen = false;
	params.HighPrecisionFPU = false;
	params.IgnoreInput = false;
	params.LoggingLevel = ELL_INFORMATION;
	params.Stencilbuffer = true;
	params.Stereobuffer = false;
	params.Vsync = false;
	// Specify which window/widget to render to
	params.WindowId = windowId;
	params.WindowSize.Width = width;
	params.WindowSize.Height = height;
	params.WithAlphaChannel = false;
	params.ZBufferBits = 16;
	// Create the Irrlicht Device with the previously specified parameters
	return createDeviceEx(params);
}

class IrrlichtSimulatorImpl  {
public:
	IrrlichtSimulatorImpl(void* windowId, int width = 640, int height = 480);
	virtual ~IrrlichtSimulatorImpl();

	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(Angle angle);
private:
	std::shared_ptr<IrrlichtRenderer> renderer;
	std::atomic<bool> terminate;
	std::atomic<bool> frameAvailable;
	std::atomic<bool> frameRequested;
	cv::Mat frame;
	std::mutex frameMutex;
	std::condition_variable frameCondition;

	virtual void start(void* windowId, int width, int height);
	virtual void stop();
};

IrrlichtSimulatorImpl::IrrlichtSimulatorImpl(void* windowId, int width, int height) :
		terminate(false), frameAvailable(false), frameRequested(false)  {
	start(windowId, width, height);
}

IrrlichtSimulatorImpl::~IrrlichtSimulatorImpl() {
	stop();
}

void IrrlichtSimulatorImpl::start(void* windowId, int width, int height) {
	std::atomic<bool> ready(false);
	std::mutex mutex;
	std::condition_variable condition;

	std::thread thread([&]() {
		using namespace irr;
		typedef IrrlichtDevice IrrDev;
		int w = width;
		int h = height;

		auto device = std::unique_ptr < IrrDev, std::function<void(IrrDev*)> > (
				windowId ? createGuestDevice(windowId, width, height) : createNativeDevice(width, height), [](IrrDev* d){ d->drop();});

		if (device) {
			renderer = std::shared_ptr < IrrlichtRenderer > (new IrrlichtRenderer(device.get()));
			{
				std::unique_lock < std::mutex > lock(mutex);
				ready = true;
				condition.notify_all();
			}
			while (!terminate && device->run()) {
				renderer->update(w, h);
				std::unique_lock < std::mutex > lock(frameMutex);
				if (frameRequested) {
					frame = renderer->captureFrame();
					frameCondition.notify_all();
					frameAvailable = true;
					frameRequested = false;
				} else {
					frameAvailable = false;
				}
			}
		}
		terminate = true;
	});
	thread.detach();

	std::unique_lock < std::mutex > lock(mutex);
	if (!ready) {
		condition.wait(lock, [&] () {return ready == true;});
	}
}

void IrrlichtSimulatorImpl::stop() {
	terminate = true;
}

cv::Mat IrrlichtSimulatorImpl::captureFrame() {
	std::unique_lock < std::mutex > lock(frameMutex);
	if (terminate) {
		throw "terminated";
	}
	if (!frameAvailable) {
		frameRequested = true;
		frameCondition.wait(lock, [this] () {return frameAvailable == true;});
	}
	return frame;
}

Speed IrrlichtSimulatorImpl::getSpeed() {
	return renderer->getSpeed();
}

void IrrlichtSimulatorImpl::setSpeed(Speed speed) {
	renderer->setSpeed(speed);
}

void IrrlichtSimulatorImpl::turn(Angle angle) {
	renderer->turn(angle);
}

IrrlichtSimulator::IrrlichtSimulator(void* windowId, int width, int height) {
	impl = std::unique_ptr < IrrlichtSimulatorImpl > (new IrrlichtSimulatorImpl(windowId, width, height));
}

IrrlichtSimulator::~IrrlichtSimulator() {
}

cv::Mat IrrlichtSimulator::captureFrame() {
	return impl->captureFrame();
}

Speed IrrlichtSimulator::getSpeed() {
	return impl->getSpeed();
}

void IrrlichtSimulator::setSpeed(Speed speed) {
	impl->setSpeed(speed);
}

void IrrlichtSimulator::turn(Angle angle) {
	impl->turn(angle);
}

} /* namespace bassma */

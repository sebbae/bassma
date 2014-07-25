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
		renderer = std::shared_ptr < IrrlichtRenderer > (new IrrlichtRenderer(windowId, width, height));
		{
			std::unique_lock < std::mutex > lock(mutex);
			ready = true;
			condition.notify_all();
		}
		while (!terminate && renderer->update(w, h)) {
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

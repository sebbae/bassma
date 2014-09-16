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

	virtual void resize(int width, int height);
	virtual cv::Mat captureFrame();

	virtual Speed getSpeed();
	virtual void setSpeed(Speed speed);
	virtual void turn(AngularVelocity velocity);
private:
	std::thread thread;
	std::shared_ptr<IrrlichtRenderer> renderer;
	std::atomic<int> width;
	std::atomic<int> height;
	std::atomic<bool> terminate;
	std::atomic<bool> frameAvailable;
	std::atomic<bool> frameRequested;
	cv::Mat frame;
	std::mutex frameMutex;
	std::condition_variable frameCondition;

	virtual void start(void* windowId);
	virtual void stop();
};

IrrlichtSimulatorImpl::IrrlichtSimulatorImpl(void* windowId, int width, int height) :
		width(width), height(height), terminate(false), frameAvailable(false), frameRequested(false)  {
	start(windowId);
}

IrrlichtSimulatorImpl::~IrrlichtSimulatorImpl() {
	stop();
}

void IrrlichtSimulatorImpl::start(void* windowId) {
	std::atomic<bool> ready(false);
	std::mutex mutex;
	std::condition_variable condition;

	thread = std::thread([&]() {
		using namespace irr;
		typedef IrrlichtDevice IrrDev;
		renderer = std::shared_ptr < IrrlichtRenderer > (new IrrlichtRenderer(windowId, width, height));
		{
			std::unique_lock < std::mutex > lock(mutex);
			ready = true;
			condition.notify_all();
		}
		while (!terminate) {
			renderer->resize(width, height);
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

	std::unique_lock < std::mutex > lock(mutex);
	if (!ready) {
		condition.wait(lock, [&] () {return ready == true;});
	}
}

void IrrlichtSimulatorImpl::stop() {
	terminate = true;
	if (thread.joinable()) {
		thread.join();
	}
}

void IrrlichtSimulatorImpl::resize(int width, int height) {
	this->width = width;
	this->height = height;
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

void IrrlichtSimulatorImpl::turn(AngularVelocity velocity) {
	renderer->turn(velocity);
}

IrrlichtSimulator::IrrlichtSimulator(void* windowId, int width, int height) {
	impl = std::unique_ptr < IrrlichtSimulatorImpl > (new IrrlichtSimulatorImpl(windowId, width, height));
}

IrrlichtSimulator::~IrrlichtSimulator() {
}

void IrrlichtSimulator::resize(int width, int height) {
	impl->resize(width, height);
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

void IrrlichtSimulator::turn(AngularVelocity velocity) {
	impl->turn(velocity);
}

} /* namespace bassma */

/*
 * IrrlichtVideoSource.cpp
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#include "IrrlichtSimulator.h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <irrlicht.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <cmath>
#include <mutex>
#include <condition_variable>
#include <GL/gl.h>
#include <X11/Xlib.h>

namespace bassma {

namespace {

using namespace irr;

enum {
	// I use this ISceneNode ID to indicate a scene node that is
	// not pickable by getSceneNodeAndCollisionPointFromRay()
	ID_IsNotPickable = 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be picked by ray selection.
	IDFlag_IsPickable = 1 << 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be highlighted.  In this example, the
	// homonids can be highlighted, but the level mesh can't.
	IDFlag_IsHighlightable = 1 << 1
};

const Acceleration accelerate = 20.0_ms2;
const Acceleration decelerate = 30.0_ms2;

const double PI = 3.14159265359;

const irr::f32 operator*(const Speed& lhs, const irr::f32& rhs) {
	return lhs.val * rhs;
}

const irr::f32 operator*(const irr::f32& lhs, const Speed& rhs) {
	return lhs * rhs.val;
}

core::vector3df yaw(Angle angle) {
	double radians = -angle.val * (PI / 180);
	return core::vector3df(std::cos(radians), 0, std::sin(radians));
}

core::vector3df tilt(Angle angle) {
	double radians = -angle.val * (PI / 180);
	return core::vector3df(std::cos(radians), std::sin(radians), 0);
}

core::vector3df roll(Angle angle) {
	double radians = (90 + angle.val) * (PI / 180);
	return core::vector3df(std::cos(radians), std::sin(radians), 0);
}

std::ostream& operator<<(std::ostream& out, const core::vector3df& v) {
	out << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
	return out;
}
}

class IrrlichtRunner {
public:
	IrrlichtRunner();
	~IrrlichtRunner();

	void run();
	void terminate();
	cv::Mat captureFrame();
	Speed getSpeed();
	void setSpeed(Speed speed);
	void turn(Angle angle);
private:
	static const int width = 640;
	static const int height = 480;

	Speed speed;
	Speed targetSpeed;
	Angle yawAngle;
	Angle tiltAngle;
	Angle rollAngle;
	std::atomic<bool> stop;
	std::atomic<bool> frameAvailable;
	std::atomic<bool> frameRequested;
	cv::Mat currentFrame;
	std::mutex mutex;
	std::condition_variable condition;

	void update(scene::ICameraSceneNode* camera, const Time frameDeltaTime);
};

IrrlichtRunner::IrrlichtRunner() :
		speed(0.0_ms), targetSpeed(0.0_ms),
		yawAngle(0.0_deg), tiltAngle(0.0_deg), rollAngle(0.0_deg),
		stop(false), frameAvailable(false), frameRequested(false) {
}

IrrlichtRunner::~IrrlichtRunner() {
	terminate();
}

void IrrlichtRunner::terminate() {
	stop.store(true);
}

Speed IrrlichtRunner::getSpeed() {
	return speed;
}

void IrrlichtRunner::setSpeed(Speed s) {
	targetSpeed = s;
}

void IrrlichtRunner::turn(Angle angle) {
}


void IrrlichtRunner::update(scene::ICameraSceneNode* camera, const Time frameDeltaTime) {
	// always move first forward/backward w.r.t. to current view direction
	core::vector3df moveAt = (camera->getTarget() - camera->getPosition()).normalize();

	Distance dist = speed * frameDeltaTime;
	if (speed < targetSpeed) {
		// account acceleration
		speed += accelerate * frameDeltaTime;
		if (speed > targetSpeed) {
			speed = targetSpeed;
		}
		dist += 0.5 * speed * frameDeltaTime;
	} else if (speed > targetSpeed) {
		// account deceleration
		speed -= accelerate * frameDeltaTime;
		if (speed < targetSpeed) {
			speed = targetSpeed;
		}
		dist += 0.5 * speed * frameDeltaTime;
	}

	core::vector3df camPosition = camera->getPosition();
	core::vector3df camPositionNew(
			camPosition.X + moveAt.X * dist.val,
			camPosition.Y + moveAt.Y * dist.val,
			camPosition.Z + moveAt.Z * dist.val);


	std::cout << "Move " << (camPositionNew - camPosition).getLength()
			<< " from " << camPosition << " into direction "
			<< moveAt << " to " << camPositionNew << " looking at "
			<< (camPositionNew + yaw(yawAngle)) << std::endl;

	camera->setPosition(camPositionNew);
	camera->setTarget(camPositionNew + yaw(yawAngle));
	camera->setUpVector(roll(rollAngle));

}


cv::Mat IrrlichtRunner::captureFrame() {
	std::unique_lock < std::mutex > lock(mutex);
	if (!frameAvailable) {
		frameRequested = true;
		condition.wait(lock, [this] () {return frameAvailable == true;});
	}
	return currentFrame;
}

void IrrlichtRunner::run() {
	using namespace irr;
	int width = 640;
	int height = 480;

	IrrlichtDevice *device = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(width, height));

	if (!device) {
		throw "Unable to create OpenGL device.";
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	const io::path workingDir = device->getFileSystem()->getWorkingDirectory();
	device->getFileSystem()->addFileArchive(workingDir + "/media/court.pk3");

	scene::IAnimatedMesh* map = smgr->getMesh("court.bsp");

	if (!map) {
		throw "Unable to load map.";
	}

	scene::ISceneNode* mapNode = smgr->addOctreeSceneNode(map->getMesh(0));

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(nullptr,
			core::vector3df(-10, 0, 0), core::vector3df(0, 0, 0),
			ID_IsNotPickable, true);
	scene::ISceneNode* ball = smgr->addSphereSceneNode(2.0);
	ball->setPosition(core::vector3df(10, 0, 0));
	ball->setMaterialTexture(0,
			driver->getTexture(workingDir + "/media/tennisball.jpg"));
	ball->setMaterialFlag(video::EMF_LIGHTING, false);

	device->getCursorControl()->setVisible(false);

	int lastFPS = -1;
	u32 then = device->getTimer()->getTime();

	while (device->run()) {
		if (device->isWindowActive()) {
			// Work out a frame delta time.
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32) (now - then) / 1000.f; // Time in seconds
			then = now;

			update(camera, Time(frameDeltaTime));

			driver->beginScene(true, true, video::SColor(255, 200, 200, 200));
			smgr->drawAll();
			driver->endScene();

			int fps = driver->getFPS();
			if (lastFPS != fps) {
				core::stringw str = L"Simulation [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;
				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}

			condition.notify_all();
		} else {
			device->yield();
		}

		std::unique_lock < std::mutex > lock(mutex);
		if (frameRequested) {
			unsigned char* buffer = new unsigned char[width * height * 3];
			glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buffer);
			cv::Mat tmp(height, width, CV_8UC3, buffer);
			cv::Mat image;
			flip(tmp, image, 0);
			currentFrame = image;
			condition.notify_all();
			frameAvailable = true;
			frameRequested = false;
		} else {
			frameAvailable = false;
		}
	}
	device->drop();
}

IrrlichtSimulator::IrrlichtSimulator() {
	runner = std::shared_ptr < IrrlichtRunner > (new IrrlichtRunner());
	std::thread thread([&]() {
		auto r = runner;
		r->run();
	});
	thread.detach();
}

IrrlichtSimulator::~IrrlichtSimulator() {
	runner->terminate();
}

cv::Mat IrrlichtSimulator::captureFrame() {
	return runner->captureFrame();
}

Speed IrrlichtSimulator::getSpeed() {
	return runner->getSpeed();
}

void IrrlichtSimulator::setSpeed(Speed speed) {
	runner->setSpeed(speed);
}

void IrrlichtSimulator::turn(Angle angle) {
	runner->turn(angle);
}

} /* namespace bassma */

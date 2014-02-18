/*
 * IrrlichtVideoSource.cpp
 *
 *  Created on: 18.02.2014
 *      Author: sebastian
 */

#include "IrrlichtSimulator.h".h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <irrlicht.h>
#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include <GL/gl.h>
#include <X11/Xlib.h>

namespace bassma {

namespace {
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
}

class IrrlichtRunner {
public:
	IrrlichtRunner();
	~IrrlichtRunner();

	void run();
	void terminate();
	cv::Mat captureFrame();
private:
	static const int width = 640;
	static const int height = 480;
	std::atomic<bool> stop;
	std::mutex mutex;
};

IrrlichtRunner::IrrlichtRunner() :
		stop(false) {
}

IrrlichtRunner::~IrrlichtRunner() {
	terminate();
}

void IrrlichtRunner::terminate() {
	stop.store(true);
}

cv::Mat IrrlichtRunner::captureFrame() {
	std::unique_lock<std::mutex> lock(mutex);
	unsigned char* buffer = new unsigned char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buffer);
	cv::Mat tmp(height, width, CV_8UC3, buffer);
	cv::Mat image;
	flip(tmp, image, 0);
	return image;
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
			core::vector3df(0, 0, 10), core::vector3df(-1, 0, 0),
			ID_IsNotPickable, true);
	scene::ISceneNode* ball = smgr->addSphereSceneNode(2.0);
	ball->setPosition(core::vector3df(-2, 0, 0));
	ball->setMaterialTexture(0,
			driver->getTexture(workingDir + "/media/tennisball.jpg"));
	ball->setMaterialFlag(video::EMF_LIGHTING, false);

	device->getCursorControl()->setVisible(false);

	int lastFPS = -1;
	const f32 MOVEMENT_SPEED = 10.f;
	u32 then = device->getTimer()->getTime();

	while (device->run()) {
		if (device->isWindowActive()) {
			std::unique_lock<std::mutex> lock(mutex);
			// Work out a frame delta time.
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32) (now - then) / 1000.f; // Time in seconds
			then = now;

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
		} else {
			device->yield();
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

} /* namespace bassma */

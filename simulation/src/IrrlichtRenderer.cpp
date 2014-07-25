/*
 * IrrlichtRenderer.cpp
 *
 *  Created on: 06.07.2014
 *      Author: sebastian
 */

#include <IrrlichtRenderer.h>

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <irrlicht.h>
#include <iostream>
#include <thread>
#include <cmath>
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

irr::IrrlichtDevice* createNativeDevice(int width, int height) {
	using namespace irr;
	return createDevice(video::EDT_OPENGL, core::dimension2d<u32>(width, height));
}

irr::IrrlichtDevice* createGuestDevice(void* windowId, int width, int height) {
	using namespace irr;
	// Set all the device creation parameters
	SIrrlichtCreationParameters params;
//	params.AntiAlias = 0;
//	params.Bits = 32;
//	params.DeviceType = EIDT_X11;
//	params.Doublebuffer = true;
//	params.DriverType = video::EDT_OPENGL;
//	params.EventReceiver = 0;
//	params.Fullscreen = false;
//	params.HighPrecisionFPU = false;
//	params.IgnoreInput = false;
//	params.LoggingLevel = ELL_INFORMATION;
//	params.Stencilbuffer = true;
//	params.Stereobuffer = false;
//	params.Vsync = false;
	// Specify which window/widget to render to
	params.WindowId = reinterpret_cast<void*>(windowId);
	params.WindowSize.Width = width;
	params.WindowSize.Height = height;
//	params.WithAlphaChannel = false;
//	params.ZBufferBits = 16;
	// Create the Irrlicht Device with the previously specified parameters
	return createDeviceEx(params);
}
}

class IrrlichtRendererImpl {
public:
	IrrlichtRendererImpl(void* windowId, int width = 640, int height = 480);
	~IrrlichtRendererImpl();

	void createScene();
	bool updateScene(int width, int height);
	void run();
	cv::Mat captureFrame();
	Speed getSpeed();
	void setSpeed(Speed speed);
	void turn(Angle angle);
private:
	int width;
	int height;
	std::unique_ptr<irr::IrrlichtDevice, std::function<void(irr::IrrlichtDevice*)>> device;
	int lastFPS;
	u32 then;
	Speed speed;
	Speed targetSpeed;
	Angle yawAngle;
	Angle tiltAngle;
	Angle rollAngle;

	void resize(int width, int height);
	void update(scene::ICameraSceneNode* camera, const Time frameDeltaTime);
};

IrrlichtRendererImpl::IrrlichtRendererImpl(void* windowId, int width, int height) :
		width(width), height(height),
		lastFPS(-1), then(0),
		speed(0.0_ms), targetSpeed(0.0_ms),
		yawAngle(0.0_deg), tiltAngle(0.0_deg), rollAngle(0.0_deg) {
	typedef irr::IrrlichtDevice IrrDev;
	device = std::unique_ptr < IrrDev, std::function<void(IrrDev*)> > (
			windowId ? createGuestDevice(windowId, width, height) : createNativeDevice(width, height), [](IrrDev* d){ d->drop();});
	createScene();
}

IrrlichtRendererImpl::~IrrlichtRendererImpl() {
}

Speed IrrlichtRendererImpl::getSpeed() {
	return speed;
}

void IrrlichtRendererImpl::setSpeed(Speed s) {
	targetSpeed = s;
}

void IrrlichtRendererImpl::turn(Angle angle) {
}


void IrrlichtRendererImpl::update(scene::ICameraSceneNode* camera, const Time frameDeltaTime) {
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

/*
	std::cout << "Move " << (camPositionNew - camPosition).getLength()
			<< " from " << camPosition << " into direction "
			<< moveAt << " to " << camPositionNew << " looking at "
			<< (camPositionNew + yaw(yawAngle)) << std::endl;
*/
	camera->setPosition(camPositionNew);
	camera->setTarget(camPositionNew + yaw(yawAngle));
	camera->setUpVector(roll(rollAngle));
}

cv::Mat IrrlichtRendererImpl::captureFrame() {
	unsigned char* buffer = new unsigned char[width * height * 3];
	glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buffer);
	cv::Mat tmp(height, width, CV_8UC3, buffer);
	cv::Mat image;
	flip(tmp, image, 0);
	return image;
}

void IrrlichtRendererImpl::createScene() {
	using namespace irr;
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
}

void IrrlichtRendererImpl::resize(int width, int height) {
	using namespace irr;
	core::dimension2d<u32> size(width, height);
	device->getVideoDriver()->OnResize(size);
	scene::ICameraSceneNode* camera = device->getSceneManager()->getActiveCamera();
	if (camera) {
		camera->setAspectRatio((f32) size.Height / (f32) size.Width);
	}
	this->width = width;
	this->height = height;
}

bool IrrlichtRendererImpl::updateScene(int width, int height) {
	using namespace irr;
	if (!device->run()) {
		return false;
	}
	if ((width != this->width) || (height != this->height)) {
		resize(width, height);
	}
	if (true || device->isWindowActive()) {
		video::IVideoDriver* driver = device->getVideoDriver();
		scene::ISceneManager* smgr = device->getSceneManager();
		scene::ICameraSceneNode* camera = smgr->getActiveCamera();

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
	} else {
		device->yield();
	}
	return true;
}

IrrlichtRenderer::IrrlichtRenderer(void* windowId, int width, int height) {
	impl = std::unique_ptr < IrrlichtRendererImpl > (new IrrlichtRendererImpl(windowId, width, height));
}

IrrlichtRenderer::~IrrlichtRenderer() {
}

bool IrrlichtRenderer::update(int width, int height) {
	return impl->updateScene(width, height);
}

cv::Mat IrrlichtRenderer::captureFrame() {
	return impl->captureFrame();
}

Speed IrrlichtRenderer::getSpeed() {
	return impl->getSpeed();
}

void IrrlichtRenderer::setSpeed(Speed speed) {
	impl->setSpeed(speed);
}

void IrrlichtRenderer::turn(Angle angle) {
	impl->turn(angle);
}

} /* namespace bassma */

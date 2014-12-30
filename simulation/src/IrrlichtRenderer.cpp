/*
 * IrrlichtRenderer.cpp
 *
 *  Created on: 06.07.2014
 *      Author: sebastian
 */

#include "IrrlichtRenderer.h"

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <irrlicht.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <./BulletCollision/CollisionDispatch/btGhostObject.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include <GL/gl.h>
#include <X11/Xlib.h>

namespace bassma {

namespace {

using namespace irr;

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
	return createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(width, height));
}

irr::IrrlichtDevice* createGuestDevice(void* windowId, int width, int height) {
	using namespace irr;
	// Set all the device creation parameters
	SIrrlichtCreationParameters params;
//	params.AntiAlias = 0;
//	params.Bits = 32;
//	params.DeviceType = EIDT_X11;
	params.Doublebuffer = true;
//	params.DriverType = video::EDT_OPENGL;
//	params.EventReceiver = 0;
//	params.Fullscreen = false;
//	params.HighPrecisionFPU = false;
//	params.IgnoreInput = false;
//	params.LoggingLevel = ELL_INFORMATION;
	params.Stencilbuffer = true;
//	params.Stereobuffer = false;
//	params.Vsync = false;
	// Specify which window/widget to render to
	params.WindowId = reinterpret_cast<void*>(windowId);
	params.WindowSize.Width = 640; // width;
	params.WindowSize.Height = 480; // height;
//	params.WithAlphaChannel = false;
//	params.ZBufferBits = 16;
			// Create the Irrlicht Device with the previously specified parameters
	return createDeviceEx(params);
}

btTriangleMesh* convertIrrMeshToBtMesh(const irr::scene::IMesh* irrMesh,
		const irr::core::vector3df& scaling) {
	btVector3 vertices[3];
	btTriangleMesh* btMesh = new btTriangleMesh();
	for (int i = 0; i < irrMesh->getMeshBufferCount(); i++) {
		irr::scene::IMeshBuffer* mb = irrMesh->getMeshBuffer(i);
		if (mb->getVertexType() == irr::video::EVT_STANDARD) {
			irr::video::S3DVertex* mb_vertices =
					static_cast<irr::video::S3DVertex*>(mb->getVertices());
			u16* mb_indices = mb->getIndices();
			u32 numIndices = mb->getIndexCount();
			for (int j = 0; j < numIndices; j += 3) {
				for (int k = 0; k < 3; k++) {
					u32 index = mb_indices[j + k];
					vertices[k] = btVector3(
							mb_vertices[index].Pos.X * scaling.X,
							mb_vertices[index].Pos.Y * scaling.Y,
							mb_vertices[index].Pos.Z * scaling.Z);
				}
				btMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		} else if (mb->getVertexType() == irr::video::EVT_2TCOORDS) {
			irr::video::S3DVertex2TCoords* mb_vertices =
					static_cast<irr::video::S3DVertex2TCoords*>(mb->getVertices());
			u16* mb_indices = mb->getIndices();
			u32 numIndices = mb->getIndexCount();
			for (int j = 0; j < numIndices; j += 3) {
				for (int k = 0; k < 3; k++) {
					u32 index = mb_indices[j + k];
					vertices[k] = btVector3(
							mb_vertices[index].Pos.X * scaling.X,
							mb_vertices[index].Pos.Y * scaling.Y,
							mb_vertices[index].Pos.Z * scaling.Z);
				}
				btMesh->addTriangle(vertices[0], vertices[1], vertices[2]);
			}
		}
	}
	return btMesh;
}

void drop(IReferenceCounted* obj) {
	obj->drop();
}

template<typename T>
using IrrObj = std::unique_ptr<T, std::function<void(IReferenceCounted*)> >;

template<typename T>
using BulletObj = std::unique_ptr<T>;

}

class IrrlichtRendererImpl {
public:
	IrrlichtRendererImpl(void* windowId, int width = 640, int height = 480);
	~IrrlichtRendererImpl();

	bool updateScene(int width, int height);
	void run();
	cv::Mat captureFrame();
	Speed getSpeed();
	void setSpeed(Speed speed);
	void turn(AngularVelocity velocity);
private:
	int width;
	int height;
	IrrObj<irr::IrrlichtDevice> device;
	BulletObj<btKinematicCharacterController> character;
	BulletObj<btPairCachingGhostObject> ghostObject;
	BulletObj<btDiscreteDynamicsWorld> world;
	BulletObj<btConstraintSolver> solver;
	BulletObj<btCollisionDispatcher> dispatcher;
	BulletObj<btBroadphaseInterface> broadPhase;
	BulletObj<btDefaultCollisionConfiguration> collisionConfig;
	std::vector<btRigidBody*> objects;
	int lastFPS;
	u32 then;
	Speed speed;
	Speed targetSpeed;
	AngularVelocity turnVelocity;
	Angle yawAngle;
	Angle tiltAngle;
	Angle rollAngle;

	void createScene();
	void destroyScene();
	void resize(int width, int height);
	void update(scene::ICameraSceneNode* camera, const Time frameDeltaTime);
	btRigidBody* addBall(scene::ISceneManager* smgr, btVector3 position);
	btRigidBody* addMap(scene::ISceneManager* smgr);
	void applyPhysics(const Time frameDeltaTime);
	void calculateMotion(const Time frameDeltaTime);
	void moveCharacter(scene::ICameraSceneNode* camera);
	void updateCamera(scene::ICameraSceneNode* camera);
};

IrrlichtRendererImpl::IrrlichtRendererImpl(void* windowId, int width,
		int height) :
		width(width), height(height), lastFPS(-1), then(0), speed(0.0_ms), targetSpeed(
				0.0_ms), turnVelocity(0.0_deg_s), yawAngle(0.0_deg), tiltAngle(
				0.0_deg), rollAngle(0.0_deg) {

	device = IrrObj<irr::IrrlichtDevice>(
			windowId ?
					createGuestDevice(windowId, width, height) :
					createNativeDevice(width, height), drop);

	createScene();
}

IrrlichtRendererImpl::~IrrlichtRendererImpl() {
	destroyScene();
}

Speed IrrlichtRendererImpl::getSpeed() {
	return speed;
}

void IrrlichtRendererImpl::setSpeed(Speed s) {
	targetSpeed = s;
}

void IrrlichtRendererImpl::turn(AngularVelocity velocity) {
	turnVelocity = velocity;
}

void IrrlichtRendererImpl::applyPhysics(const Time frameDeltaTime) {
	//std::cout << "Delta time " << (frameDeltaTime.val) << std::endl;
	world->stepSimulation(frameDeltaTime.val * 4, 60);
	// Relay the object's orientation to irrlicht
	for (btRigidBody* body : objects) {
		irr::scene::ISceneNode* node =
				static_cast<irr::scene::ISceneNode*>(body->getUserPointer());
		// Set position
		btVector3 point = body->getCenterOfMassPosition();
		core::vector3df newPos((f32) (point[0]), (f32) (point[1]),
				(f32) (point[2]));
		//std::cout << "Update object to " << newPos << std::endl;
		node->setPosition(newPos);
		// Set rotation
		core::vector3df Euler;
		const btQuaternion& TQuat = body->getOrientation();
		irr::core::quaternion q(TQuat.getX(), TQuat.getY(), TQuat.getZ(),
				TQuat.getW());
		q.toEuler(Euler);
		Euler *= irr::core::RADTODEG;
		node->setRotation(Euler);
	}
}

void IrrlichtRendererImpl::calculateMotion(const Time frameDeltaTime) {
	if (speed < targetSpeed) {
		// account acceleration
		speed += accelerate * frameDeltaTime;
		if (speed > targetSpeed) {
			speed = targetSpeed;
		}
	} else if (speed > targetSpeed) {
		// account deceleration
		speed -= accelerate * frameDeltaTime;
		if (speed < targetSpeed) {
			speed = targetSpeed;
		}
	}
	yawAngle = yawAngle + turnVelocity * frameDeltaTime;
}

void IrrlichtRendererImpl::moveCharacter(scene::ICameraSceneNode* camera) {
	// always move first forward/backward w.r.t. to current view direction
	core::vector3df direction = camera->getTarget() - camera->getPosition();
	core::vector3df moveAt = direction.normalize();
	btVector3 walkDirection = btVector3(moveAt.X, moveAt.Y, moveAt.Z)
			* speed.val;
	character->setWalkDirection(
			btVector3(moveAt.X, moveAt.Y, moveAt.Z) * speed.val);
}

void IrrlichtRendererImpl::updateCamera(scene::ICameraSceneNode* camera) {
	btVector3 pos = ghostObject->getWorldTransform().getOrigin();
	core::vector3df camPosition = camera->getPosition();
	core::vector3df camPositionNew = core::vector3df(pos.getX(), pos.getY(),
			pos.getZ());
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

void IrrlichtRendererImpl::update(scene::ICameraSceneNode* camera,
		const Time frameDeltaTime) {
	calculateMotion(frameDeltaTime);
	moveCharacter(camera);
	applyPhysics(frameDeltaTime);
	updateCamera(camera);
}

cv::Mat IrrlichtRendererImpl::captureFrame() {
	if (true) {
		// take screenshot via Irrlicht API
		typedef std::unique_ptr<irr::video::IImage> IImageUPtr;
		IImageUPtr img = IImageUPtr(
				device->getVideoDriver()->createScreenShot());
		irr::core::dimension2d<unsigned int> dim = img->getDimension();

		// copy pixels to OpenCV BGR image
		cv::Mat tmp(dim.Height, dim.Width, CV_8UC3);
		for (int x = 0; x < dim.Width; x++) {
			for (int y = 0; y < dim.Height; y++) {
				cv::Vec3b& bgrPixel = tmp.at<cv::Vec3b>(y, x);
				irr::video::SColor p = img->getPixel(x, y);
				bgrPixel[0] = p.getBlue();
				bgrPixel[1] = p.getGreen();
				bgrPixel[2] = p.getRed();
			}
		}
		return tmp;
	} else {
		// copy OpenGL buffer to OpenCV BGR image
		unsigned char* buffer = new unsigned char[width * height * 3];
		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, buffer);
		cv::Mat tmp(height, width, CV_8UC3, buffer);
		cv::Mat image;
		flip(tmp, image, 0); // OpenGL stores image different than OpenCV
		return image;
	}
}

btRigidBody* IrrlichtRendererImpl::addBall(scene::ISceneManager* smgr,
		btVector3 position) {
	btScalar radius = 2.0;
	btScalar mass = 1;

	const io::path workingDir = device->getFileSystem()->getWorkingDirectory();
	scene::IMeshSceneNode* ball = smgr->addSphereSceneNode(radius, 32);
	ball->setPosition(core::vector3df(10, 0, 0));
	ball->setMaterialTexture(0,
			device->getVideoDriver()->getTexture(
					workingDir + "/media/tennisball.jpg"));
	ball->setMaterialFlag(video::EMF_LIGHTING, false);

	// Set the initial position of the object
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(position);

	btDefaultMotionState* state = new btDefaultMotionState(transform);

	// Create the shape
	btCollisionShape* shape = new btSphereShape(radius);

	// Add mass
	btVector3 intertia;
	shape->calculateLocalInertia(mass, intertia);

	// Create the rigid body object
	btRigidBody* rigidBody = new btRigidBody(mass, state, shape, intertia);
	rigidBody->setFriction(5.5f);
	rigidBody->setRestitution(1.0f);

	// Store a pointer to the irrlicht node so we can update it later
	rigidBody->setUserPointer(ball);

	return rigidBody;
}

btRigidBody* IrrlichtRendererImpl::addMap(scene::ISceneManager* smgr) {
	const io::path workingDir = device->getFileSystem()->getWorkingDirectory();
	device->getFileSystem()->addFileArchive(workingDir + "/media/court.pk3");
	scene::IAnimatedMesh* mesh = smgr->getMesh("court.bsp");
	if (!mesh) {
		throw "Unable to load map.";
	}
	scene::IMeshSceneNode* map = smgr->addOctreeSceneNode(mesh->getMesh(0));

	btTriangleMesh* indexVertexArrays = convertIrrMeshToBtMesh(map->getMesh(),
			map->getScale());
	btBvhTriangleMeshShape* trimeshShape = new btBvhTriangleMeshShape(
			indexVertexArrays, true);

	btQuaternion quat;
	quat.setEulerZYX(0, 0, 0);
	btTransform Transform2;
	Transform2.setIdentity();
	Transform2.setOrigin(btVector3(0, 0, 0));
	Transform2.setRotation(quat);

	btDefaultMotionState* motionState = new btDefaultMotionState(Transform2);
	btRigidBody* rigidBody = new btRigidBody(0, motionState, trimeshShape);
	rigidBody->setUserPointer(map);

	rigidBody->setRestitution(0.8f);
	rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

	return rigidBody;
}

void IrrlichtRendererImpl::createScene() {
	using namespace irr;
	using namespace irr::scene;
	using namespace irr::core;
	ISceneManager* smgr = device->getSceneManager();
	device->getCursorControl()->setVisible(false);

	// Initialize bullet
	collisionConfig = BulletObj<btDefaultCollisionConfiguration>(
			new btDefaultCollisionConfiguration());
	broadPhase = BulletObj<btBroadphaseInterface>(
			new btAxisSweep3(btVector3(-1000, -1000, -1000),
					btVector3(1000, 1000, 1000)));
	dispatcher = BulletObj<btCollisionDispatcher>(
			new btCollisionDispatcher(collisionConfig.get()));
	solver = BulletObj<btConstraintSolver>(
			new btSequentialImpulseConstraintSolver());
	world = BulletObj<btDiscreteDynamicsWorld>(
			new btDiscreteDynamicsWorld(dispatcher.get(), broadPhase.get(),
					solver.get(), collisionConfig.get()));

	world->setGravity(btVector3(0, -9.8, 0));

	auto map = addMap(smgr);
	world->addRigidBody(map);

	auto ball = addBall(smgr, btVector3(10, 0, 0));
	world->addRigidBody(ball);
	objects.push_back(ball);

	auto ball2 = addBall(smgr, btVector3(7, 5, 5));
	world->addRigidBody(ball2);
	objects.push_back(ball2);

	irr::core::vector3df cameraPos = vector3df(-30, -32, 0);
	ICameraSceneNode* camera = smgr->addCameraSceneNode(nullptr, cameraPos,
			vector3df(0, 0, 0), 0, true);

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(cameraPos.X, cameraPos.Y, cameraPos.Z));

	ghostObject = std::unique_ptr<btPairCachingGhostObject>(
			new btPairCachingGhostObject());
	ghostObject->setWorldTransform(startTransform);
	world->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(
			new btGhostPairCallback());
	btScalar characterHeight = 1.75;
	btScalar characterWidth = 1.75;
	btConvexShape* capsule = new btCapsuleShape(characterWidth,
			characterHeight);
	ghostObject->setCollisionShape(capsule);
	ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0);
	character = std::unique_ptr<btKinematicCharacterController>(
			new btKinematicCharacterController(ghostObject.get(), capsule,
					stepHeight));

	world->addCollisionObject(ghostObject.get(),
			btBroadphaseProxy::CharacterFilter,
			btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
	world->addAction(character.get());
}

void IrrlichtRendererImpl::destroyScene() {
	world->removeCollisionObject(ghostObject.get());
	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--) {
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		world->removeCollisionObject(obj);
		delete obj;
	}
}

void IrrlichtRendererImpl::resize(int width, int height) {
	using namespace irr;
	core::dimension2d<u32> size(width, height);
	scene::ICameraSceneNode* camera =
			device->getSceneManager()->getActiveCamera();
	if (camera) {
		const irr::core::rect<signed int> viewPort =
				device->getVideoDriver()->getViewPort();
		std::cout << "viewport (" << viewPort.LowerRightCorner.X << "," << viewPort.LowerRightCorner.Y << ") (" << viewPort.UpperLeftCorner.X << "," << viewPort.UpperLeftCorner.Y << std::endl;
		device->getVideoDriver()->setViewPort(core::rect<s32>(0,0,width, height));
		//camera->setAspectRatio((f32) size.Width / (f32) size.Height);
	}
	device->getVideoDriver()->OnResize(size);
	this->width = width;
	this->height = height;
}

bool IrrlichtRendererImpl::updateScene(int width, int height) {
	using namespace irr;
	if (!device->run()) {
		return false;
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

		if ((width != this->width) || (height != this->height)) {
			resize(width, height);
		}
		driver->beginScene(true, true, video::SColor(0, 200, 200, 200));
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
	impl = std::unique_ptr<IrrlichtRendererImpl>(
			new IrrlichtRendererImpl(windowId, width, height));
}

IrrlichtRenderer::~IrrlichtRenderer() {
}

void IrrlichtRenderer::resize(int width, int height) {
	impl->updateScene(width, height);
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

void IrrlichtRenderer::turn(AngularVelocity velocity) {
	impl->turn(velocity);
}

} /* namespace bassma */

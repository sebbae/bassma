#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <irrlicht.h>
#include <iostream>
#include <GL/gl.h>

using namespace irr;
using namespace cv;

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

int main(int argc, char* argv[]) {
	int width = 640;
	int height = 480;

	IrrlichtDevice *device = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(width, height));

	if (!device) {
		std::cerr << "Unable to create OpenGL device.";
		return 1;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	const io::path workingDir = device->getFileSystem()->getWorkingDirectory();
	device->getFileSystem()->addFileArchive(workingDir + "/src/media/court.pk3");

	scene::IAnimatedMesh* map = smgr->getMesh("court.bsp");

	if (!map) {
		std::cerr << "Unable to load map.";
		return 1;
	}

	scene::ISceneNode* node = smgr->addOctreeSceneNode(map->getMesh(0));
	//node->setPosition(core::vector3df(0, 0, 0));

	scene::ICameraSceneNode* camera = smgr->addCameraSceneNode(nullptr,
			core::vector3df(0, 0, 10), core::vector3df(-1, 0, 0),
			ID_IsNotPickable, true);
	/*
	 smgr->addCameraSceneNodeFPS(nullptr,
	 100.0f, .3f, ID_IsNotPickable, nullptr, 0, true, 3.f);
	 camera->setPosition(core::vector3df(0, 0, 10));
	 camera->setTarget(core::vector3df(0, 1, 0));
	 */
	/*
	 scene::ITriangleSelector* selector = smgr->createOctreeTriangleSelector(
	 map->getMesh(0), node, 128);
	 node->setTriangleSelector(selector);
	 scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
	 selector, camera, core::vector3df(10, 10, 10),
	 core::vector3df(0, -10, 0), core::vector3df(0, 30, 0));
	 selector->drop();
	 camera->addAnimator(anim);
	 anim->drop();
	 */

	device->getCursorControl()->setVisible(false);

	int lastFPS = -1;
	const f32 MOVEMENT_SPEED = 10.f;
	u32 then = device->getTimer()->getTime();

	while (device->run()) {
		if (device->isWindowActive()) {
			// Work out a frame delta time.
			const u32 now = device->getTimer()->getTime();
			const f32 frameDeltaTime = (f32) (now - then) / 1000.f; // Time in seconds
			then = now;

//			core::vector3df cameraPosition = camera->getPosition();
//			core::vector3df cameraTarget = camera->getTarget();
//			cameraPosition.X -= MOVEMENT_SPEED * frameDeltaTime;
//			camera->setPosition(cameraPosition);
//			camera->setTarget(cameraPosition + core::vector3df(-1,0,0));

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

			if (true) {
				unsigned char* buffer = new unsigned char[width*height*3];
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
				Mat tmp(height, width, CV_8UC3, buffer);
				Mat image;
				flip(tmp, image, 0);
				cvtColor(image, image, CV_RGB2BGR);

				if (!image.data) {
					printf("No image data \n");
					return -1;
				}

				namedWindow("Display Image", CV_WINDOW_AUTOSIZE);
				imshow("Display Image", image);
				cv::waitKey(50);
			}

		} else {
			device->yield();
		}
	}

	device->drop();
	return 0;
}

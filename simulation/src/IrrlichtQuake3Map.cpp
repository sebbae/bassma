#include <irrlicht.h>
#include <iostream>

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

int main(int argc, char* argv[]) {
	IrrlichtDevice *device = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(640, 480));

	if (!device) {
		std::cerr << "Unable to create OpenGL device.";
		return 1;
	}

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	device->getFileSystem()->addFileArchive(
			"/home/sebastian/projects/bassma/git/simulation/media/court.pk3");

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

			core::vector3df cameraPosition = camera->getPosition();
			core::vector3df cameraTarget = camera->getTarget();
			cameraPosition.X -= MOVEMENT_SPEED * frameDeltaTime;
			camera->setPosition(cameraPosition);
			camera->setTarget(cameraPosition + core::vector3df(-1,0,0));

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
	return 0;
}

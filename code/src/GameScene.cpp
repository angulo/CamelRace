/* 
 * GameScene.cpp -- Game scene implementation file
 *
 * Copyright (C) 2013 Javier Angulo Lucerón <javier.angulo1@gmail.com>
 * 
 * This file is part of CamelRace
 *
 * CamelRace is free software: you can redistribute it and/or modify it under the terms of 
 * the GNU General Public License as published by the Free Software Foundation, either 
 * version 3 of the License, or (at your option) any later version.
 * CamelRace is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with CamelRace. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "GameScene.h"

using namespace CamelRace;

std::string
GameScene::_checkFinishLine()
{
	static bool onX = false;
	static bool onY = false;

	Ogre::Vector3 camelPosition = _camel->getTrackingNode()->getPosition();

	Ogre::Ray ray(camelPosition, Ogre::Vector3(0, 1, 0));

	// It establishes the ray for the query
	Ogre::RaySceneQuery *raySceneQuery = _sceneManager->createRayQuery(ray);
	raySceneQuery->setSortByDistance(true);
	raySceneQuery->setQueryMask(2);

	Ogre::RaySceneQueryResult &result = raySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator it;

	Ogre::MovableObject *collision = NULL;

	std::string position = "";
	bool nowOnX = false;
	bool nowOnY = false;

	for (it = result.begin(); it != result.end(); it++) {
		collision = it->movable;
		if (collision->getName() == "finishX") {
			nowOnX = true;
		} else if (collision->getName() == "finishY") {
			nowOnY = true;
		}
	}
	
	if (!nowOnX && onX) {
		position = "X";
	} else if (!nowOnY && onY) {
		position = "Y";
	}

	onX = nowOnX;
	onY = nowOnY;

	return position;
}

void
GameScene::_createCircuit()
{
	//Lights
	_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	_sceneManager->setShadowTextureCount(2);
	_sceneManager->setShadowTextureSize(512);

	_sceneManager->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5) );
	_sceneManager->setAmbientLight(Ogre::ColourValue(0.7, 0.7, 0.7));
	
	Ogre::Light *sunLight = _sceneManager->createLight("Sun");
	sunLight->setPosition(1000, 1000, 0);
	sunLight->setType(Ogre::Light::LT_DIRECTIONAL);
	sunLight->setDirection(1, -1, 0);
	sunLight->setSpotlightInnerAngle(Ogre::Degree(25.0f));
	sunLight->setSpotlightOuterAngle(Ogre::Degree(100.0f));
	sunLight->setSpotlightFalloff(0.0);
	sunLight->setCastShadows(true);

	// Sky
	OGF::ModelBuilderPtr builder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager));

	Ogre::Entity *skyEntity = builder->modelPath("sky_dome.mesh")
		->queryFlags(1)
		->buildEntity();
	skyEntity->setMaterialName("sky");
	
	Ogre::SceneNode *skyNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
	skyNode->attachObject(skyEntity);
	skyNode->translate(0, -200, 0);

	// Cameras
	_topCamera = _sceneManager->createCamera("TopCamera");

	_topCamera->setPosition(_configValue<float>("camera_x"), _configValue<float>("camera_y"), _configValue<float>("camera_z"));
	_topCamera->lookAt(0, 0, 0);

	_topCamera->setNearClipDistance(0.1);
	_topCamera->setFarClipDistance(10000);
	_topCamera->setFOVy(Ogre::Degree(75));

	_topCameraNode = _sceneManager->createSceneNode("TopCameraNode");
	_topCameraNode->attachObject(_topCamera);
	_topCameraNode->setInheritOrientation(false);

	Ogre::Viewport *viewport;
	Ogre::RenderWindow *renderWindow = Ogre::Root::getSingletonPtr()->getAutoCreatedWindow();

	if (renderWindow->getNumViewports() > 0) {
		viewport = renderWindow->getViewport(0);
		viewport->setCamera(_topCamera);
	} else {
		viewport = renderWindow->addViewport(_topCamera);
	}

	_topCamera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) /
		Ogre::Real(viewport->getActualHeight()));
}

void
GameScene::_createDynamicWorld()
{
	_debugDrawer = new OgreBulletCollisions::DebugDrawer();
	_debugDrawer->setDrawWireframe(true);	 

	Ogre::SceneNode *node = _sceneManager->getRootSceneNode()->
		createChildSceneNode("debugNode", Ogre::Vector3::ZERO);
	node->attachObject(static_cast <Ogre::SimpleRenderable *>(_debugDrawer));

	Ogre::AxisAlignedBox worldBounds = Ogre::AxisAlignedBox(
		Ogre::Vector3(-1000, -1000, -1000), 
		Ogre::Vector3(1000,  1000,  1000)
	);

	Ogre::Vector3 gravity(0, -9.8, 0);

	_world = new OgreBulletDynamics::DynamicsWorld(_sceneManager, worldBounds, gravity);
	_world->setDebugDrawer(_debugDrawer);
	_world->setShowDebugShapes(false);

	// Circuit
	Ogre::SceneNode *circuitNode = _sceneManager->getRootSceneNode()->createChildSceneNode();

	OGF::ModelBuilderPtr builder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager, Model::PLANE));
	builder->castShadows(false)
		->queryFlags(1)
		->parent(circuitNode);

	OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL; 
	OgreBulletDynamics::RigidBody *rigidBody = NULL;

	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();
	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath(OGF::ModelFactory::getSingletonPtr()->getPath(Model::CIRCUIT_INTERN))
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("circuitIntern", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);
	delete trimeshConverter;

	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();
	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath(OGF::ModelFactory::getSingletonPtr()->getPath(Model::CIRCUIT_EXTERN))
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("circuitExtern", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();

	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath(OGF::ModelFactory::getSingletonPtr()->getPath(Model::PLANE))
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("circuit", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();

	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath("start.mesh")
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("startLine", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();

	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath("stones.mesh")
				->nodeName("")
				->buildNode()->getAttachedObject(0)
		)
	);


	rigidBody = new OgreBulletDynamics::RigidBody("stones", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();

	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath("fence.mesh")
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("fence", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();
	builder->castShadows(true);

	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath("palms.mesh")
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("palms", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);
	delete trimeshConverter;

	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter();
	trimeshConverter->addEntity(
		static_cast<Ogre::Entity *>(
			builder->modelPath("house.mesh")
				->buildNode()->getAttachedObject(0)
		)
	);

	rigidBody = new OgreBulletDynamics::RigidBody("house", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);
	delete trimeshConverter;
}

void
GameScene::_createFinishLines()
{
	OGF::ModelBuilderPtr builder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager));

	builder->castShadows(true)
	->queryFlags(2)
	->visible(false)
	->modelPath("finish_x.mesh")
	->entityName("finishX")
	->nodeName("finishX")
	->position(Ogre::Vector3(0, 0, 0))
	->parent(_sceneManager->getRootSceneNode()->createChildSceneNode())
	->buildNode();
	
	builder->castShadows(true)
	->queryFlags(2)
	->visible(false)
	->modelPath("finish_y.mesh")
	->entityName("finishY")
	->nodeName("finishY")
	->position(Ogre::Vector3(0, 0, 0))
	->parent(_sceneManager->getRootSceneNode()->createChildSceneNode())
	->buildNode();
}

void
GameScene::_createScene()
{
	_createCircuit();
	_createDynamicWorld();
	_createFinishLines();
	_camel = new CamelWidget(_sceneManager, _world);
	_camelChildId = OGF::SceneController::getSingletonPtr()->addChild(_camel);

	Ogre::SceneNode *camelNode = _camel->getTrackingNode();
	camelNode->addChild(_topCameraNode);

	_counter = new Ogre::Timer();
}

void
GameScene::_loadOverlay()
{
	Ogre::FontManager *fontManager = Ogre::FontManager::getSingletonPtr();
	fontManager->getByName("MoanLisa")->load();

	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();

	Ogre::Overlay *statsOverlay = overlayManager->getByName("Stats");
	statsOverlay->show();
}

void
GameScene::_updateCounter(const std::string &camelPosition)
{
	_finishLinesState[_finishLinesPointer] = camelPosition;
	_finishLinesPointer = (_finishLinesPointer + 1) % 2;

	if (_finishLinesState[0] == "Y" && _finishLinesState[1] == "X") {
		_finishLinesState[0] = _finishLinesState[1] = "_";
		if (_counter->getMilliseconds() < _bestTime || _bestTime == 0) {
			_bestTime = _counter->getMilliseconds();
			Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();

			Ogre::OverlayElement *time = overlayManager->getOverlayElement("bestTimeValue");
			time->setCaption(Ogre::StringConverter::toString(static_cast<int>(_bestTime / 1000)));
		}
		_counter->reset();
	}
}

void
GameScene::_updateCounterOverlay()
{
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();

	Ogre::OverlayElement *time = overlayManager->getOverlayElement("timeValue");
	time->setCaption(Ogre::StringConverter::toString(static_cast<int>(_counter->getMilliseconds() / 1000)));
}

GameScene::GameScene()
{
	_initConfigReader("scenes/game.cfg");
	_finishLinesState.push_back("_");
	_finishLinesState.push_back("_");
	_finishLinesPointer = 0;
	_bestTime = 0;
}

GameScene::~GameScene()
{
	_soundMainTheme->stop();

	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	Ogre::Overlay *statsOverlay = overlayManager->getByName("Stats");
	statsOverlay->hide();
}

void
GameScene::enter()
{
	_createScene();
	_loadOverlay();

	_soundMainTheme = OGF::SoundTrackManager::getSingletonPtr()->load("theme.mp3");
	_soundMainTheme->play(true);
}

void
GameScene::exit()
{
	_soundMainTheme->stop();

	_sceneManager->destroyAllCameras();
	_sceneManager->destroyAllStaticGeometry();
	_sceneManager->destroyAllMovableObjects();
	_sceneManager->destroyAllAnimationStates();
	_sceneManager->destroyAllLights();

	Ogre::SceneNode *root = _sceneManager->getRootSceneNode();
	root->removeAndDestroyAllChildren();
}

void
GameScene::pause()
{

}

void
GameScene::resume()
{

}

bool
GameScene::frameStarted(const Ogre::FrameEvent& event)
{
	static float timeSinceLastCheck = 0;

	_world->stepSimulation(event.timeSinceLastFrame);

	// Prevent the orientation of the camera to follow the car
	Ogre::Quaternion orientation = _camel->getTrackingNode()->getOrientation();
	orientation.z = 0;
	orientation.x = 0;
	Ogre::SceneNode *s;
	_topCameraNode->resetOrientation();
	_topCameraNode->setOrientation(orientation);

	timeSinceLastCheck += event.timeSinceLastFrame;
	if (timeSinceLastCheck >= 0.25) {
		timeSinceLastCheck = 0;
		std::string camelPosition = _checkFinishLine();
		if (camelPosition.size() > 0) {
			_updateCounter(camelPosition);
		}
	}

	_updateCounterOverlay();

	return true;
}

bool
GameScene::frameEnded(const Ogre::FrameEvent& event)
{
	_world->stepSimulation(event.timeSinceLastFrame);

	return true;
}

bool
GameScene::keyPressed(const OIS::KeyEvent& event)
{
	if (event.key == OIS::KC_ESCAPE) {
		OGF::SceneController::getSingletonPtr()->pop();
	}

	return true;
}

bool
GameScene::keyReleased(const OIS::KeyEvent& event)
{
	return true;
}

bool
GameScene::mouseMoved(const OIS::MouseEvent& event)
{
	return true;
}

bool
GameScene::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID buttonId)
{
	return true;
}

bool
GameScene::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID buttonId)
{
	return true;
}

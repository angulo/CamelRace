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

void
GameScene::_createCircuit()
{
	// Cameras
	_topCamera = _sceneManager->createCamera("TopCamera");

	_topCamera->setPosition(_configValue<float>("camera_x"), _configValue<float>("camera_y"), _configValue<float>("camera_z"));
	_topCamera->lookAt(0, 0, 0);

	_topCamera->setNearClipDistance(10);
	_topCamera->setFarClipDistance(250);
	_topCamera->setFOVy(Ogre::Degree(90));

	Ogre::SceneNode *topCameraNode = _sceneManager->createSceneNode("TopCameraNode");
	topCameraNode->attachObject(_topCamera);

	_sceneManager->getRootSceneNode()->addChild(topCameraNode);

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
	
	//Lights
	_sceneManager->setAmbientLight(Ogre::ColourValue(1, 1, 1));
	_sceneManager->createLight("Light")->setPosition(30, 10, 10);

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
	OGF::ModelBuilderPtr builder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager, Model::PLANE));

	Ogre::SceneNode *planeNode = builder->castShadows(true)
		->parent(_sceneManager->getRootSceneNode())
		->position(Ogre::Vector3(0, 0, 0))
		->buildNode();

	Ogre::Entity *planeEntity = static_cast<Ogre::Entity *>(planeNode->getAttachedObject(0));

	OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL; 
	OgreBulletCollisions::CollisionShape *bodyShape = NULL;
	OgreBulletDynamics::RigidBody *rigidBody = NULL;

	trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(planeEntity);
	bodyShape = trimeshConverter->createConvex();
	delete trimeshConverter;

	rigidBody = new OgreBulletDynamics::RigidBody("circuit", _world);
	rigidBody->setStaticShape(planeNode, bodyShape, 0.1, 0.8, Ogre::Vector3(0, -10, 0), Ogre::Quaternion::IDENTITY);
}

void
GameScene::_createScene()
{
	_createCircuit();
	_createDynamicWorld();
	_camel = new CamelWidget(_sceneManager, _world);
	_camelChildId = OGF::SceneController::getSingletonPtr()->addChild(_camel);
}

GameScene::GameScene()
{
	_initConfigReader("scenes/game.cfg");
}

GameScene::~GameScene()
{

}

void
GameScene::enter()
{
	_createScene();
}

void
GameScene::exit()
{

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
	_world->stepSimulation(event.timeSinceLastFrame);
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

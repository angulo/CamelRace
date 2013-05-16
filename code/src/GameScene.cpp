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
		->buildEntity();
	skyEntity->setMaterialName("sky");
	
	Ogre::SceneNode *skyNode = _sceneManager->getRootSceneNode()->createChildSceneNode ();
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
	_world->setShowDebugShapes(true);

	// Circuit
	Ogre::SceneNode *circuitNode = _sceneManager->getRootSceneNode()->createChildSceneNode();

	OGF::ModelBuilderPtr builder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager, Model::PLANE));
	builder->castShadows(false)
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
				->buildNode()->getAttachedObject(0)
		)
	);


	rigidBody = new OgreBulletDynamics::RigidBody("stones", _world);
	rigidBody->setStaticShape(circuitNode, trimeshConverter->createTrimesh(), 0.1, 0.8, Ogre::Vector3(0, 0, 0), Ogre::Quaternion::IDENTITY);

	delete trimeshConverter;
	builder->castShadows(true);
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
GameScene::_createScene()
{
	_createCircuit();
	_createDynamicWorld();
	_camel = new CamelWidget(_sceneManager, _world);
	_camelChildId = OGF::SceneController::getSingletonPtr()->addChild(_camel);

	_camel->getTrackingNode()->addChild(_topCameraNode);
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

	// Prevent the orientation of the camel to change
	Ogre::Quaternion orientation = _camel->getTrackingNode()->getOrientation();
	orientation.z = 0;
	orientation.x = 0;
	Ogre::SceneNode *s;
	_topCameraNode->resetOrientation();
	_topCameraNode->setOrientation(orientation);

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

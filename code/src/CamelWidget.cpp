/* 
 * CamelWidget.cpp -- Camel element implementation file
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

#include "CamelWidget.h"

using namespace CamelRace;

void
CamelWidget::_updateDirection(const Ogre::FrameEvent& event)
{
	static float steering = 0;	

	OIS::Keyboard *keyboard = OGF::InputManager::getSingletonPtr()->getKeyboard();
	
	float modification = 0;

	if (keyboard->isKeyDown(OIS::KC_LEFT)) {
		if (steering < 0.4)
			modification = event.timeSinceLastFrame * 0.15;
	} else if (keyboard->isKeyDown(OIS::KC_RIGHT)) {
		if (steering > -0.4)
			modification = -(event.timeSinceLastFrame * 0.15);
	} else {
		modification = event.timeSinceLastFrame * 0.5;
		if (steering > 0) {
			modification = - std::min(steering, modification);		
		} else if (steering < 0){
			modification = - std::max(steering, -modification);		
		}

	}

	steering += modification;

	_vehicle->setSteeringValue(steering, 0); 
	_vehicle->setSteeringValue(steering, 1); 
}

void
CamelWidget::_updatePower(const Ogre::FrameEvent& event)
{
	OIS::Keyboard *keyboard = OGF::InputManager::getSingletonPtr()->getKeyboard();

	float engineForce = 30000;

	_vehicle->applyEngineForce(0, 0);
	_vehicle->applyEngineForce(0, 1);

	if (keyboard->isKeyDown(OIS::KC_UP)) {
		_vehicle->applyEngineForce(engineForce, 0);
		_vehicle->applyEngineForce(engineForce, 1);
	} else if (keyboard->isKeyDown(OIS::KC_DOWN)) {
		_vehicle->applyEngineForce(-engineForce * 0.75, 0);
		_vehicle->applyEngineForce(-engineForce * 0.75, 1);
	}
}

CamelWidget::CamelWidget(Ogre::SceneManager *sceneManager, OgreBulletDynamics::DynamicsWorld *world)
	: Scene(sceneManager), _world(world)
{
	_initConfigReader("scenes/game.cfg");
}

CamelWidget::~CamelWidget()
{

}

Ogre::SceneNode *
CamelWidget::getTrackingNode()
{
	return _trackingNode;
}

OgreBulletCollisions::Object *
CamelWidget::getCollisionObject()
{
	return _world->findObject(_vehicle->getBulletVehicle()->getRigidBody());
}

void
CamelWidget::enter()
{
	const Ogre::Vector3 chassisShift(0, 1.0, 0);
	float connectionHeight = 0.7f;
	float gWheelRadius = 0.5f;
	float gWheelWidth = 0.4f;
	float gWheelFriction = 0.8f;
	float gRollInfluence = 0.1f;
	float gSuspensionRestLength = 0.6;

	_trackingNode = _sceneManager->getRootSceneNode()->createChildSceneNode ();

	OGF::ModelBuilderPtr chassisBuilder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager, Model::CHASSIS));

	Ogre::SceneNode *chassisNode = chassisBuilder->castShadows(true)
		->parent(_trackingNode)
		->position(chassisShift)
		->nodeName("chassis")
		->queryFlags(1)
		->buildNode();
	
	OgreBulletCollisions::BoxCollisionShape *chassisShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1.f, 0.75f, 2.1f));
	OgreBulletCollisions::CompoundCollisionShape *compound = new OgreBulletCollisions::CompoundCollisionShape();

	compound->addChildShape(chassisShape, chassisShift); 

	OgreBulletDynamics::WheeledRigidBody  *carChassis = new  OgreBulletDynamics::WheeledRigidBody("carChassis", _world);

	Ogre::Vector3 position(_configValue<float>("camel_x"), 3, _configValue<float>("camel_z")); 

	carChassis->setShape(_trackingNode, compound, 0.6, 0.6, 800, position, Ogre::Quaternion(0.7, 0, 0.7, 0));
	carChassis->setDamping(0.2, 0.2);
	carChassis->disableDeactivation();

	OgreBulletDynamics::VehicleTuning *tuning = new OgreBulletDynamics::VehicleTuning(20.2, 4.4, 2.3, 500.0, 10.5);

	OgreBulletDynamics::VehicleRayCaster *vehicleRayCaster = new OgreBulletDynamics::VehicleRayCaster(_world);

	_vehicle = new OgreBulletDynamics::RaycastVehicle(carChassis, tuning, vehicleRayCaster);

	_vehicle->setCoordinateSystem(0, 1, 2);

	Ogre::Entity *wheelEntities[4];
	Ogre::SceneNode *wheelNodes[4];

	OGF::ModelBuilderPtr wheelBuilder(OGF::ModelFactory::getSingletonPtr()->getBuilder(_sceneManager, Model::WHEEL));

	wheelBuilder->castShadows(true)
		->queryFlags(1)
		->parent(_sceneManager->getRootSceneNode()->createChildSceneNode ());

	for (size_t i = 0; i < 4; i++) {
		wheelNodes[i] = wheelBuilder->buildNode();
	}

	bool isFrontWheel = true;
	Ogre::Vector3 connectionPoint(1 - (0.3 * gWheelWidth), connectionHeight, 2 - gWheelRadius);
	Ogre::Vector3 wheelDirectionCS0(0, -1, 0);
	Ogre::Vector3 wheelAxleCS(-1, 0, 0);

	_vehicle->addWheel(wheelNodes[0], connectionPoint, wheelDirectionCS0, wheelAxleCS, gSuspensionRestLength, gWheelRadius, isFrontWheel, gWheelFriction, gRollInfluence);

	connectionPoint = Ogre::Vector3(-1 + (0.3 * gWheelWidth), connectionHeight, 2 - gWheelRadius);
	_vehicle->addWheel(wheelNodes[1], connectionPoint, wheelDirectionCS0, wheelAxleCS, gSuspensionRestLength, gWheelRadius, isFrontWheel, gWheelFriction, gRollInfluence);

	isFrontWheel = false;

	connectionPoint = Ogre::Vector3(-1 + (0.3 * gWheelWidth), connectionHeight, -2 + gWheelRadius);
	_vehicle->addWheel(wheelNodes[2], connectionPoint, wheelDirectionCS0, wheelAxleCS, gSuspensionRestLength, gWheelRadius, isFrontWheel, gWheelFriction, gRollInfluence);

	connectionPoint = Ogre::Vector3(1 - (0.3 * gWheelWidth), connectionHeight, -2 + gWheelRadius);
	_vehicle->addWheel(wheelNodes[3], connectionPoint, wheelDirectionCS0, wheelAxleCS, gSuspensionRestLength, gWheelRadius, isFrontWheel, gWheelFriction, gRollInfluence);

	_vehicle->setSteeringValue(0, 0); 
	_vehicle->setSteeringValue(0, 1); 
}

void
CamelWidget::exit()
{

}

void
CamelWidget::pause()
{

}

void
CamelWidget::resume()
{

}

bool
CamelWidget::frameStarted(const Ogre::FrameEvent& event)
{
	_updateDirection(event);
	_updatePower(event);

	return true;
}

bool
CamelWidget::frameEnded(const Ogre::FrameEvent& event)
{
	return true;
}

bool
CamelWidget::keyPressed(const OIS::KeyEvent& event)
{
	return true;
}

bool
CamelWidget::keyReleased(const OIS::KeyEvent& event)
{
	return true;
}

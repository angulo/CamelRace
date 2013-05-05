/* 
 * CamelWidget.h -- Camel element header file
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

#ifndef _CAMEL_WIDGET_H_
#define _CAMEL_WIDGET_H_

#include <OGF/OGF.h>

#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>

#include <OgreBulletDynamicsWorld.h>
#include <OgreBulletDynamicsRigidBody.h>

#include <Constraints/OgreBulletDynamicsRaycastVehicle.h>

#include "Model.h"
#include "UIConfig.h"

namespace CamelRace {
	
	class CamelWidget : public OGF::Scene {

		protected:

			OgreBulletDynamics::DynamicsWorld *_world;
			OgreBulletDynamics::RaycastVehicle *_vehicle;

			void _updateDirection(const Ogre::FrameEvent& event);
			void _updatePower(const Ogre::FrameEvent& event);

		public:
			
			CamelWidget(Ogre::SceneManager *sceneManager, OgreBulletDynamics::DynamicsWorld * world);
			~CamelWidget();

			void enter();
			void exit();
			void pause();
			void resume();

			bool frameStarted(const Ogre::FrameEvent& event);
			bool frameEnded(const Ogre::FrameEvent& event);

			bool keyPressed(const OIS::KeyEvent& event);
			bool keyReleased(const OIS::KeyEvent& event);
	};
};

#endif

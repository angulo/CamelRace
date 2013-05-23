/* 
 * GameScene.h -- Game scene header file
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

#ifndef _GAME_SCENE_H_
#define _GAME_SCENE_H_

#include <OGF/OGF.h>
#include <OGRE/OgreFontManager.h>
#include <vector>

#include <Debug/OgreBulletCollisionsDebugDrawer.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"		

#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

#include <OgreBulletDynamicsWorld.h>
#include <OgreBulletDynamicsRigidBody.h>

#include "CamelWidget.h"
#include "Model.h"
#include "SceneFactory.h"
#include "UIConfig.h"

namespace CamelRace {
	
	class GameScene : public OGF::Scene {
		
		protected:
			
			Ogre::Camera *_topCamera;
			Ogre::SceneNode *_topCameraNode;

			OgreBulletCollisions::DebugDrawer * _debugDrawer;
			OgreBulletDynamics::DynamicsWorld * _world;

			CamelWidget *_camel;
			OGF::ChildId _camelChildId;

			Ogre::Timer *_counter;
			unsigned long _bestTime;
			
			std::vector<std::string> _finishLinesState; 
			int _finishLinesPointer;

			OGF::SoundTrackPtr _soundMainTheme;

			std::string _checkFinishLine();
			void _createCircuit();
			void _createFinishLines();
			void _createDynamicWorld();
			void _createScene();
			void _loadOverlay();
			void _updateCounter(const std::string &camelPosition);
			void _updateCounterOverlay();
		
		public:
			
			GameScene();
			~GameScene();

			void enter();
			void exit();
			void pause();
			void resume();

			bool frameStarted(const Ogre::FrameEvent& event);
			bool frameEnded(const Ogre::FrameEvent& event);

			bool keyPressed(const OIS::KeyEvent& event);
			bool keyReleased(const OIS::KeyEvent& event);

			bool mouseMoved(const OIS::MouseEvent& event);
			bool mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID buttonId);
			bool mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID buttonId);
	};
};

#endif

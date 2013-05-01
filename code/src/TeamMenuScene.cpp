/* 
 * TeamMenuScene.cpp -- Team menu scene implementation file
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

#include "TeamMenuScene.h"

using namespace CamelRace;

TeamMenuScene::TeamMenuScene()
{
	_initConfigReader("scenes/team_menu.cfg");
}

TeamMenuScene::~TeamMenuScene()
{
}

void
TeamMenuScene::enter()
{
	_container = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "TeamMenuContainer");

	_windowBackground = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
	_windowBackground->setProperty("Image", "set:Menus image:Team");
	_windowBackground->setProperty("FrameEnabled", "False");

	_container->addChildWindow(_windowBackground);

	CEGUI::System::getSingletonPtr()->setGUISheet(_container);
}

void
TeamMenuScene::exit()
{
	CEGUI::WindowManager::getSingletonPtr()->destroyWindow(_container);
}

void
TeamMenuScene::pause()
{
	_container->setVisible(false);
}

void
TeamMenuScene::resume()
{
	CEGUI::System::getSingletonPtr()->setGUISheet(_container);
	_container->setVisible(true);
}

bool
TeamMenuScene::keyPressed(const OIS::KeyEvent &event)
{
	switch(event.key) {

		case OIS::KC_RETURN:
			OGF::SceneController::getSingletonPtr()->pop();
			break;

		default:
			break;
	}
	
	return true;
}

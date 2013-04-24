/* 
 * MainMenuScene.cpp -- Main menu scene implementation file
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

#include "MainMenuScene.h"

using namespace CamelRace;

void
MainMenuScene::_setCurrentOption(MainMenuOption::Option option)
{
	// Reset the previously selected option
	CEGUI::Window *currentOptionWindow = _optionsMap[_currentOption];
	CEGUI::String currentOptionText = currentOptionWindow->getText();
	currentOptionWindow->setText(currentOptionText.replace(9, 8, 
		MENU_OPTION_COLOR_UNSELECTED));

	_currentOption = option;

	// Set the new option
	currentOptionWindow = _optionsMap[_currentOption];
	currentOptionText = currentOptionWindow->getText();
	currentOptionWindow->setText(currentOptionText.replace(9, 8, 
		MENU_OPTION_COLOR_SELECTED));
}

CEGUI::Window *
MainMenuScene::_createOptionWindow(const std::string &text, const int &x, const int &y)
{
	CEGUI::Window *result = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
	result->setProperty("Text", CEGUI::String("[colour='") + MENU_OPTION_COLOR_UNSELECTED + "']" + text);
	result->setProperty("Font", MENU_OPTION_FONT);
	result->setProperty("UnifiedAreaRect", "{{0," + Ogre::StringConverter::toString(x) 
		+ "},{0," + Ogre::StringConverter::toString(y) + "},{1,0},{1,0}}");

	return result;
}

MainMenuScene::MainMenuScene()
	:	_currentOption(MainMenuOption::PLAY)
{
	_initConfigReader("scenes/main_menu.cfg");
}

MainMenuScene::~MainMenuScene()
{
}

void
MainMenuScene::enter()
{
	_windowBackground = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
	_windowBackground->setProperty("Image", "set:Menus image:Main");
	_windowBackground->setProperty("FrameEnabled", "False");

	_optionsMap[MainMenuOption::PLAY] = _createOptionWindow("PLAY", 100, 80);
	_optionsMap[MainMenuOption::RECORDS] = _createOptionWindow("HIGH SCORES", 100, 80);
	_optionsMap[MainMenuOption::OPTIONS] = _createOptionWindow("OPTIONS", 100, 80);
	_optionsMap[MainMenuOption::HELP] = _createOptionWindow("HELP", 100, 80);
	_optionsMap[MainMenuOption::EXIT] = _createOptionWindow("EXIT", 100, 80);

	for (std::map<MainMenuOption::Option, CEGUI::Window *>::iterator it =
		_optionsMap.begin(); it != _optionsMap.end(); it++) {

		_windowBackground->addChildWindow(it->second);
	}

	CEGUI::System::getSingletonPtr()->setGUISheet(_windowBackground);
	_setCurrentOption(MainMenuOption::PLAY);
}

void
MainMenuScene::_processCurrentOption()
{
	switch(_currentOption) {
		case MainMenuOption::PLAY:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::GAME);
			break;
		case MainMenuOption::RECORDS:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::MENU_RECORDS);
			break;
		case MainMenuOption::OPTIONS:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::MENU_OPTIONS);
			break;
		case MainMenuOption::HELP:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::MENU_HELP);
			break;
		case MainMenuOption::EXIT:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::EXIT);
			break;
	}
}

void
MainMenuScene::exit()
{

}

void
MainMenuScene::pause()
{
	_windowBackground->setVisible(false);
}

void
MainMenuScene::resume()
{
	_windowBackground->setVisible(true);
}

bool
MainMenuScene::keyPressed(const OIS::KeyEvent &event)
{
	switch(event.key) {

		case OIS::KC_DOWN:
			{
				MainMenuOption::Option newOption = static_cast<MainMenuOption::Option>(_currentOption - 1);
				if (newOption < 0)
					newOption = static_cast<MainMenuOption::Option>(_optionsMap.size());
				_setCurrentOption(newOption);
			}
			break;

		case OIS::KC_UP:
			{
				MainMenuOption::Option newOption = static_cast<MainMenuOption::Option>((_currentOption + 1) % _optionsMap.size());
				_setCurrentOption(newOption);
			}
			break;

		case OIS::KC_RETURN:
			_processCurrentOption();

		default:
			break;
	}
	
	return true;
}

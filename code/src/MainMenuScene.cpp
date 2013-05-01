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
		_configValue<std::string>("font_color_unselected")));

	_currentOption = option;

	// Set the new option
	currentOptionWindow = _optionsMap[_currentOption];
	currentOptionText = currentOptionWindow->getText();
	currentOptionWindow->setText(currentOptionText.replace(9, 8, 
		_configValue<std::string>("font_color_selected")));
}

CEGUI::Window *
MainMenuScene::_createOptionWindow(const std::string &text, const std::string &font, const int &x, const int &y)
{
	CEGUI::Window *result = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
	result->setProperty("Text", CEGUI::String("[colour='") + _configValue<std::string>("font_color_unselected") + "']" + text);
	result->setProperty("Font", font);
	result->setProperty("UnifiedAreaRect", "{{0," + Ogre::StringConverter::toString(x) 
		+ "},{0," + Ogre::StringConverter::toString(y) + "},{1,0},{1,0}}");
	result->setProperty("FrameEnabled", "False");

	return result;
}

MainMenuScene::MainMenuScene()
	:	_currentOption(MainMenuOption::PLAY), _exit(false)
{
	_initConfigReader("scenes/main_menu.cfg");
	CEGUI::FontManager::getSingleton().create("MoanLisa-Big.font");
	CEGUI::FontManager::getSingleton().create("MoanLisa-Small.font");
}

MainMenuScene::~MainMenuScene()
{
}

void
MainMenuScene::enter()
{
	_container = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", "MainMenuContainer");

	_windowBackground = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage");
	_windowBackground->setProperty("Image", "set:Menus image:Main");
	_windowBackground->setProperty("FrameEnabled", "False");

	_container->addChildWindow(_windowBackground);

	_optionsMap[MainMenuOption::PLAY] = _createOptionWindow("PLAY", _configValue<std::string>("font_big"), _configValue<int>("play_x"), _configValue<int>("play_y"));
	_optionsMap[MainMenuOption::OPTIONS] = _createOptionWindow("OPTIONS", _configValue<std::string>("font_small"),  _configValue<int>("options_x"), _configValue<int>("options_y"));
	_optionsMap[MainMenuOption::TEAM] = _createOptionWindow("TEAM", _configValue<std::string>("font_small"),  _configValue<int>("help_x"), _configValue<int>("help_y"));
	_optionsMap[MainMenuOption::EXIT] = _createOptionWindow("EXIT", _configValue<std::string>("font_small"),  _configValue<int>("exit_x"), _configValue<int>("exit_y"));

	for (std::map<MainMenuOption::Option, CEGUI::Window *>::iterator it =
		_optionsMap.begin(); it != _optionsMap.end(); it++) {

		_container->addChildWindow(it->second);
	}

	CEGUI::System::getSingletonPtr()->setGUISheet(_container);
	_setCurrentOption(MainMenuOption::PLAY);
}

void
MainMenuScene::_processCurrentOption()
{
	switch(_currentOption) {
		case MainMenuOption::PLAY:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::GAME);
			break;
		case MainMenuOption::OPTIONS:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::MENU_OPTIONS);
			break;
		case MainMenuOption::TEAM:
			OGF::SceneController::getSingletonPtr()->push(CamelRace::Scene::MENU_TEAM);
			break;
		case MainMenuOption::EXIT:
			_exit = true;
			break;
	}
}

void
MainMenuScene::exit()
{
	CEGUI::WindowManager::getSingletonPtr()->destroyWindow(_container);
}

void
MainMenuScene::pause()
{
	_container->setVisible(false);
}

void
MainMenuScene::resume()
{
	CEGUI::System::getSingletonPtr()->setGUISheet(_container);
	_container->setVisible(true);
}

bool
MainMenuScene::frameStarted(const Ogre::FrameEvent& event)
{
	return !_exit;
}

bool
MainMenuScene::keyPressed(const OIS::KeyEvent &event)
{
	switch(event.key) {

		case OIS::KC_LEFT:
		case OIS::KC_DOWN:
			{
				MainMenuOption::Option newOption;

				if (_currentOption == 0) {
					newOption = static_cast<MainMenuOption::Option>(_optionsMap.size() - 1);
				} else {
					newOption = static_cast<MainMenuOption::Option>(_currentOption - 1);
				}

				_setCurrentOption(newOption);
			}
			break;

		case OIS::KC_RIGHT:
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

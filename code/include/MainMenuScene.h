/* 
 * MainMenuScene.h -- Main menu scene header file
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

#ifndef _MAIN_MENU_SCENE_H_
#define _MAIN_MENU_SCENE_H_

#include <OGF/OGF.h>

#include "SceneFactory.h"
#include "UIConfig.h"

namespace CamelRace {
	
	namespace MainMenuOption {

		enum Option {
			PLAY = 0,
			OPTIONS = 1,
			TEAM = 2,
			EXIT = 3
		};
	};

	class MainMenuScene : public OGF::Scene {
		
		private:
			
			CEGUI::Window *_windowBackground;
			CEGUI::Window *_container;

			std::map<MainMenuOption::Option, CEGUI::Window *> _optionsMap;
			MainMenuOption::Option _currentOption;
			bool _exit;

			void _setCurrentOption(MainMenuOption::Option option);
			CEGUI::Window * _createOptionWindow(const std::string &text, const std::string &font, const int &x, const int &y);
			void _processCurrentOption();

		public:
			
			MainMenuScene();
			~MainMenuScene();

			void enter();
			void exit();
			void pause();
			void resume();

			bool frameStarted(const Ogre::FrameEvent& event);
			bool keyPressed(const OIS::KeyEvent &event);
	};
};

#endif

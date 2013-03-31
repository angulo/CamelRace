/* 
 * main.cpp -- Main file of the project with system initialization
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

#include <OGF/OGF.h>

#include "SceneFactory.h"
#include "UIConfig.h"

int
main(int argc, char **argv)
{
	OGF::ISceneFactory *sceneFactory = new CamelRace::SceneFactory();

	if (!OGF::Bootstrap::getSingletonPtr()->init("resources.cfg", APPLICATION_WINDOW_NAME, sceneFactory, CamelRace::Scene::MENU_MAIN)) {
		return 1;
	}

	OGF::Bootstrap::getSingletonPtr()->run();
	OGF::Bootstrap::getSingletonPtr()->shutdown();

	return 0;
}

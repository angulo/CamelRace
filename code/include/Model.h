/* 
 * Model.h -- model ids definition and modelfactory initializing
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

#ifndef _MODEL_H_
#define _MODEL_H_

#include <OGF/OGF.h>

namespace CamelRace {
	
	namespace Model {

		enum ModelList {
			CHASSIS,
			CIRCUIT_INTERN,
			CIRCUIT_EXTERN,
			PLANE,
			WHEEL
		};

		OGF::ModelMap getModelMap();
	};
};

#endif

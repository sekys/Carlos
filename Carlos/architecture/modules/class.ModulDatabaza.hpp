#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulDatabaza : public IModul  {
	public:

		// Metoda ma hladat blizke objekty vo svete pdola GPS suradnic
		virtual vector<WorldObject> najdiVsetkySvetoveObjektyBlizkoGPS(GPS gps) {
			vector<WorldObject> zoznam;
			// Stiahni vsetko z datbazi
			WorldObject world;
			world.id = 1;
			world.position.latitude = 10.f;
			world.position.longitude = 10.f;
			world.cestyKSuborom.push_back("C:/deskriptor.desc");
			world.cestyKSuborom.push_back("C:/deskriptor1.desc");
			world.cestyKSuborom.push_back("C:/deskriptor2.desc");

			zoznam.push_back(world);
			return zoznam;
		}

	};
}




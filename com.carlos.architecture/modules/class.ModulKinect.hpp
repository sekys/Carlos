#pragma once
#include "../../libs/include/carlos_global.h"
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulKinect : public IModul  {
	public:
		// Ziskaj aktualny uhol hlavy (defaultna pozicia) v milimetroch
		virtual Point3f getAktualnaRotaciaHlavy() {
			Point3f rot;
			rot.x = 270.0f; // 27cm
			rot.y = 160.0f; // 16cm
			rot.z = 400.0f; // 40cm
			return rot;
		}

		/**
		* Tato metoda sa spusti, ked pouzivatel stlacil tlacidlo a je
		* potrebne zkonfigurovat kinect.
		*/
		virtual void spustiKalibraciu() {

		}


	};
}




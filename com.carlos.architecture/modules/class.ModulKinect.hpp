#pragma once
#include "../../libs/include/carlos_global.h"
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulKinect : public IModul  {
	public:
		// Ziskaj aktualny uhol hlavy
		virtual Point3f getAktualnaRotaciaHlavy() {
			Point3f rot;
			rot.x = 10.f;
			rot.y = 10.f;
			rot.z = 14.f;
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




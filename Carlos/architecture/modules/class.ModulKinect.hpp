#pragma once
#include "..\entities\entities.h"
#include "..\entities\class.IModul.hpp"

namespace Architecture
{
	class ModulKinect : public IModul  {
	public:
		virtual Rotation getAktualnaRotaciaHlavy() {
			Rotation rot;
			rot.rotation[0] = 10.f;
			rot.rotation[1] = 10.f;
			rot.rotation[2] = 14.f;
			return rot;
		}

	};
}




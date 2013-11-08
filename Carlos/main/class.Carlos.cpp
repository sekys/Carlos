#include "class.Carlos.hpp"

using namespace Architecture;

void Carlos::spracujJedenSnimok(Image& image) {

	// Za gps suradnicov sa musi synchronizovane pockat, potom sa moze ist dalej
	// Lebo ked snimka meska, tak gps moze byt uz o par metrov dalej
	GPS gps = controller->android->getAktualnaPozicia();
	Rotation rotaciaHlavy = controller->kinect->getAktualnaRotaciaHlavy();

	// Modul preprocessingu
	vector<WorldObject> recepts = controller->databaza->najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania = controller->spracovania->detekujObjekty(spracovanie);

	//! Tu uz je potrebna rotaciaHlavy

	// Modul vypoctu polohy
	vector<Position> najdenePozicie; // sycnhronizovane
	for(uint i=0; i < vysledokSpracovania.objects.size(); i++) {
		ModulVypocitaniaPolohy::In vypocetPolohy;
		vypocetPolohy.gps = gps;
		vypocetPolohy.polohaObjektu = vysledokSpracovania.objects.at(i).position;
		vypocetPolohy.rotaciaHlavy = rotaciaHlavy;

		ModulVypocitaniaPolohy::Out polohaTextu;
		polohaTextu = controller->vyppolohy->vypocitajPolohuTextu(vypocetPolohy);
		if(polohaTextu.najdeny) {
			najdenePozicie.push_back(polohaTextu.polohaTextu);
		}
	}

	// Modul vykreslovania
	ModulVykreslovania::In vykreslovanie;
	vykreslovanie.image = image;
	vykreslovanie.najdenePozicie = najdenePozicie;
	controller->vykreslovania->vykresliObrazokSRozsirenouRealitou(vykreslovanie);
	imshow("Test", image.data);
}


void Carlos::spracujSnimky() {
	namedWindow("Test",1);
	Image frame;
	frame.frame = 0;
	
	while(1)
	{
		ControllerCommands command = controller->android->getActualCommand();
		// input->getActualCommand();
		
		// threads
		// while s pevnym deltacasom
		// GameLogic triedu
		// cele toto dat do view co sa prave ukazuje

		controller->kamera->readNext(frame);
		spracujJedenSnimok(frame);
		frame.frame++;
		// Bum ! Hotovo mozme ist na dalsi snimok
	}

	// Android podystem ovladania bude bezat v inej funkcii a v inom threade
}
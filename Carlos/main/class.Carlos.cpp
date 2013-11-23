#include "class.Carlos.hpp"
#include "../architecture/configuration/class.Configuration.h"
using namespace Architecture;

Carlos::Carlos() {
	db = new DBService();
	controller = new ModulesController();
}

Carlos::~Carlos() {
	controller->stop();
	SAFE_DELETE(controller);
	SAFE_DELETE(db);
}

//
void Carlos::spracujJedenSnimok(Image& image) {
	// Z gps suradnic sa musi synchronizovane pockat, potom sa moze ist dalej
	// Lebo ked snimka meska, tak gps moze byt uz o par metrov dalej
	GPS gps = controller->android->getGPS();
	Rotation rotaciaHlavy = controller->kinect->getAktualnaRotaciaHlavy();

	// Modul preprocessingu
	vector<WorldObject> recepts = controller->databaza->najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania = controller->spracovania->detekujObjekty(spracovanie);

	// Modul vypoctu polohy
	vector<Position> najdenePozicie; // synchronizovane
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


void Carlos::Init() {
	db->selectObjects();
	Configuration::getInstance();
	cout << "Configuration title '" << Configuration::getInstance().getTitle() << "'\n";
	controller->start();
	namedWindow("Test",1);
	frame.frame = 0;
}
bool Carlos::Run() {
	controller->callPreFrames();
	nacitajDalsiuSnimku();
	return true;
}

void Carlos::nacitajDalsiuSnimku() {
	ControllerCommands command = controller->android->getActualCommand();
	// threads
	// while s pevnym deltacasom
	// GameLogic triedu
	// cele toto dat do view co sa prave ukazuje

	controller->kamera->readNext(frame);
	spracujJedenSnimok(frame);
	frame.frame++;
	// Bum ! Hotovo mozme ist na dalsi snimok
}
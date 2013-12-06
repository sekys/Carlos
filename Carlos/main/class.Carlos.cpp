#include "class.Carlos.hpp"
#include "../architecture/configuration/class.Configuration.h"
using namespace Architecture;

Carlos::Carlos() {
	// Nacitaj vsetky casti Carlosa
	db = new DBService();
	controller = new ModulesController();
}


Carlos::~Carlos() {
	// Program konci, je potrebne uvolnit jeho casti
	controller->stop();
	SAFE_DELETE(controller);
	SAFE_DELETE(db);
}


void Carlos::spracujJedenSnimok(Image& image) {
	// Z gps suradnic sa musi synchronizovane pockat, potom sa moze ist dalej
	// Lebo ked snimka meska, tak gps moze byt uz o par metrov dalej
	ControllerCommands command = controller->android->getActualCommand();
	GPS gps = controller->android->getGPS();
	Point3f rotaciaHlavy = controller->kinect->getAktualnaRotaciaHlavy();

	// Modul preprocessingu
	vector<WorldObject> recepts = controller->databaza->najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania = controller->spracovanie->detekujObjekty(spracovanie);

	// Modul vypoctu polohy
	vector<Point2f> najdenePozicie; // synchronizovane
	for(uint i=0; i < vysledokSpracovania.objects.size(); i++) {
		ModulVypocitaniaPolohy::In vypocetPolohy;
		vypocetPolohy.gps = gps;
		vypocetPolohy.polohaObjektu = vysledokSpracovania.objects.at(i).boundary;
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
	vykreslovanie.command = command;
	vykreslovanie.najdenePozicie = najdenePozicie;
	controller->vykreslovanie->vykresliObrazokSRozsirenouRealitou(vykreslovanie);
	imshow("Test", image.data);
}


void Carlos::Init() {
	// Inicializacia Carlosu spociva napriklad v nacitani konfiguracie ...
	db->selectObjects();
	Configuration::getInstance();
	cout << "Configuration title '" << Configuration::getInstance().getTitle() << "'\n";

	// Spociva aj v nacitani modulov
	controller->start();
	namedWindow("Test",1);
	frame.frame = 0;
}
bool Carlos::Run() {
	// Tato metoda sa spusta v kazdom cykle apliakcie
	controller->callPreFrames();
	nacitajDalsiuSnimku();
	return true;
}

void Carlos::nacitajDalsiuSnimku() {
	// Ziskaj snimok ..
	controller->kamera->readNext(frame);
	spracujJedenSnimok(frame);
	frame.frame++;

	// Bum ! Hotovo mozme ist na dalsi snimok
}
#include <carlos_global.h>
#include "class.Carlos.hpp"
#include "../../com.carlos.architecture/configuration/class.Configuration.hpp"
#include "../../com.carlos.architecture/db/class.DBService.hpp"
using namespace Architecture;

Carlos::Carlos() {
	// Nacitaj vsetky casti Carlosa
	controller = new ModulesController();
}


Carlos::~Carlos() {
	// Program konci, je potrebne uvolnit jeho casti
	controller->stop();
	SAFE_DELETE(controller);
}


void Carlos::spracujJedenSnimok(Image image) {
	// Z gps suradnic sa musi synchronizovane pockat, potom sa moze ist dalej
	// Lebo ked snimka meska, tak gps moze byt uz o par metrov dalej
	ControllerCommands command = controller->android->getActualCommand();
	controller->android->setActualCommand(ControllerCommands::NO_ACTION);
	GPS gps = controller->android->getGPS();
	Point3f rotaciaHlavy = controller->kinect->getAktualnaRotaciaHlavy();
	imshow("Vstup", image.data);

	// Modul preprocessingu
	vector<WorldObject> recepts;
	// recepts = DB::DBService::getInstance().najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania;
	// vysledokSpracovania = controller->spracovanie->detekujObjekty(spracovanie);
	vysledokSpracovania.horizont = controller->spracovanie->najdiHorizont(image.data);
	imshow("Horizont", vysledokSpracovania.horizont);

	// Modul vypoctu polohy
	vector<ModulVypocitaniaPolohy::Out> najdeneObjekty; // synchronizovane
	/*for(uint i=0; i < vysledokSpracovania.objects.size(); i++) {
		ModulVypocitaniaPolohy::In vypocetPolohy;
		vypocetPolohy.id = vysledokSpracovania.objects.at(i).objekt.id;
		vypocetPolohy.gps = gps;
		vypocetPolohy.polohaObjektu = vysledokSpracovania.objects.at(i).boundary;
		vypocetPolohy.rotaciaHlavy = rotaciaHlavy;

		ModulVypocitaniaPolohy::Out polohaTextu;
		polohaTextu = controller->vyppolohy->vypocitajPolohuTextu(vypocetPolohy);
		if(polohaTextu.najdeny) {
			najdeneObjekty.push_back(polohaTextu);
		}
	}*/

	// Modul vykreslovania
	ModulVykreslovania::In* vykreslovanie;
	vykreslovanie = new ModulVykreslovania::In();
	vykreslovanie->image = image;
	vykreslovanie->command = command;
	vykreslovanie->najdeneObjekty = najdeneObjekty;
	vykreslovanie->horizont = vysledokSpracovania.horizont;
	controller->vykreslovanie->vykresliObrazokSRozsirenouRealitou(vykreslovanie);


}


void Carlos::Init() {
	// Inicializacia Carlosu spociva napriklad v nacitani konfiguracie ...
	//db->selectObjects();
	DB::DBService::getInstance();
	Configuration::getInstance();
	cout << "Configuration title '" << Configuration::getInstance().getTitle() << "'\n";

	// Spociva aj v nacitani modulov
	controller->start();
	namedWindow("Vstup", WND_PROP_FULLSCREEN);
	namedWindow("Horizont", WND_PROP_FULLSCREEN);
}
bool Carlos::Run() {
	// Tato metoda sa spusta v kazdom cykle apliakcie
	controller->callPreFrames();
	nacitajDalsiuSnimku();
	return true; // okno obnovujeme stale
}

void Carlos::nacitajDalsiuSnimku() {
	// Ziskaj snimok ..
	try {
		Image image;
		image = controller->kamera->readNext();
		cout << "Snimok: " << image.frame << "\n";
		spracujJedenSnimok(image);
	} catch(ModulKamera::EndOfStream stream) {
		// Cyklus Run skonci, skonci apliakcia, spusti sa dekonstruktor, zacne sa uvolnovat pamet a vsetko vypinat ...
		this->stop();
	} catch (std::out_of_range e) {
		cout << e.what();
	} 
}
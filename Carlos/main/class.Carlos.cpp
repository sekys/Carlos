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

	// Modul preprocessingu
	vector<WorldObject> recepts = DB::DBService::getInstance().najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania = controller->spracovanie->detekujObjekty(spracovanie);
	vysledokSpracovania.horizont = controller->spracovanie->najdiHorizont(image.data);

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
	vykreslovanie.horizont = vysledokSpracovania.horizont;
	controller->vykreslovanie->vykresliObrazokSRozsirenouRealitou(vykreslovanie);
	//imshow("Horizont", vysledokSpracovania.horizont);
	imshow("Test", image.data);
	image.data.release();
}


void Carlos::Init() {
	// Inicializacia Carlosu spociva napriklad v nacitani konfiguracie ...
	//db->selectObjects();
	DB::DBService::getInstance();
	Configuration::getInstance();
	cout << "Configuration title '" << Configuration::getInstance().getTitle() << "'\n";

	// Spociva aj v nacitani modulov
	controller->start();
	namedWindow("Test",1);
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
		controller->kamera->readNext();
		Image image = controller->kamera->getImage();
		//cout << "Snimok: " << image.frame << "\n";
		spracujJedenSnimok(image);
	} catch(ModulKamera::EndOfStream stream) {
		// Cyklus Run skonci, skonci apliakcia, spusti sa dekonstruktor, zacne sa uvolnovat pamet a vsetko vypinat ...
		this->stop();
	} catch (std::out_of_range e) {
		cout << e.what();
	} 
}
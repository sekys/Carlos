#include <carlos_global.h>
#include "class.Carlos.hpp"
#include "../../com.carlos.architecture/configuration/class.Configuration.hpp"
#include "../../com.carlos.architecture/db/class.DBService.hpp"
using namespace Architecture;

Carlos::Carlos()  {
	// Nacitaj vsetky casti Carlosa
	log = CREATE_LOG4CPP();
	if(log != NULL) {
		log->debug("Starting carlos");
	}
	controller = new ModulesController();
}

Carlos::~Carlos() {
	// Program konci, je potrebne uvolnit jeho casti
	if(log != NULL) log->debug("Ending carlos");
	controller->stop();
	SAFE_DELETE(controller);
}

int Carlos::getLockFPS() {
	return m_lockFPS;
}

void Carlos::spracujJedenSnimok(Image image) {

	// Z gps suradnic sa musi synchronizovane pockat, potom sa moze ist dalej
	// Lebo ked snimka meska, tak gps moze byt uz o par metrov dalej
	controller->android->prepareFakeGPS(image.pos_msec);

	// toto bude musiet ist do hry
	ControllerCommands command = controller->android->getActualCommand();
	controller->android->setActualCommand(ControllerCommands::NO_ACTION);
	GPS gps = controller->android->getGPS();
	Point3f rotaciaHlavy = controller->kinect->getAktualnaRotaciaHlavy();
	// + horizont
	//\ toto bude musiet ist do hry

	//treba nastavit podla velkosti rozlisenia monitora

	imshow("Vstup", image.data);
	//resizeWindow("Vstup", 1280,768);
	if(m_fullscreen) {
		cvSetWindowProperty("Vstup", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
		cv::moveWindow("Vstup", 0, 0);
	}
	spracujVstupy(image, command, gps, rotaciaHlavy);
	image.data.release();
}

void Carlos::spracujVstupy(Image image, ControllerCommands command, GPS gps, Point3f rotaciaHlavy) {
	// Modul preprocessingu
	vector<WorldObject> recepts;
	recepts = DB::DBService::getInstance().najdiVsetkySvetoveObjektyBlizkoGPS(gps);

	// Modul spracovania
	ModulSpracovania::In spracovanie;
	spracovanie.image = image;
	spracovanie.recepts = recepts;
	ModulSpracovania::Out vysledokSpracovania;
	//vysledokSpracovania = controller->spracovanie->detekujObjekty(spracovanie);
	vysledokSpracovania.horizont = controller->spracovanie->najdiHorizont(image.data);
	//imshow("Horizont", vysledokSpracovania.horizont);

	// Modul vypoctu polohy
	vector<ModulVypocitaniaPolohy::Out> najdeneObjekty; // synchronizovane
	for (uint i=0; i < vysledokSpracovania.objects.size(); i++) {
		ModulVypocitaniaPolohy::In vypocetPolohy;
		vypocetPolohy.id = vysledokSpracovania.objects.at(i).objekt.id;
		vypocetPolohy.gps = gps;

		GPS objectGPS;
		const DB::Object *object = DB::DBService::getInstance().getObjectById(vypocetPolohy.id);

		objectGPS.latitude = object->latitude;
		objectGPS.longitude = object->longitude;

		vypocetPolohy.gpsPolohaObjektu = objectGPS;
		vypocetPolohy.polohaObjektu = vysledokSpracovania.objects.at(i).boundary;
		vypocetPolohy.rotaciaHlavy = rotaciaHlavy;

		ModulVypocitaniaPolohy::Out polohaTextu;
		polohaTextu = controller->vyppolohy->vypocitajPolohuTextu(vypocetPolohy);

		if(polohaTextu.najdeny) {
			najdeneObjekty.push_back(polohaTextu);
		}
	}

	// Modul vykreslovania
	ModulVykreslovania::In* vykreslovanie;
	vykreslovanie = new ModulVykreslovania::In();
	vykreslovanie->command = command;
	vykreslovanie->position = controller->vyppolohy->getHeadPosition(rotaciaHlavy); // tu by mi mala prist pozicia - cislo z intervalu -1,1
	vykreslovanie->najdeneObjekty = najdeneObjekty;
	vykreslovanie->horizont = vysledokSpracovania.horizont;
	controller->vykreslovanie->vykresliObrazokSRozsirenouRealitou(vykreslovanie);
}

void Carlos::Init() {
	// Inicializacia Carlosu spociva napriklad v nacitani konfiguracie ...
	DB::DBService::getInstance();
	if(log != NULL) {
		log->debugStream() << "Configuration title '" << Configuration::getInstance().getTitle();
	}

	m_fullscreen = Configuration::getInstance().getConfigi("fullscreen") == 1;
	m_lockFPS = Configuration::getInstance().getConfigi("lock_fps");

	// Spociva aj v nacitani modulov
	controller->start();

	if(m_fullscreen) {
		namedWindow("Vstup");
	} else {
		namedWindow("Vstup", WND_PROP_FULLSCREEN);
	}
	// namedWindow("Horizont", WND_PROP_FULLSCREEN);
}

bool Carlos::Run() {
	// Tato metoda sa spusta v kazdom cykle apliakcie
	// controller->callPreFrames();
	nacitajDalsiuSnimku();
	return true; // okno obnovujeme stale
}

void Carlos::nacitajDalsiuSnimku() {
	// Ziskaj snimok ..
	Image image;
	try {
		try {
			image = controller->kamera->readNext();
		} catch(ModulKamera::EndOfStream stream) {
			// Tu nastane 5sec delay ked je koniec streamu, dovod neznamy
			if(log != NULL) {
				log->debugStream() << "Restartujem stream.";
			}
			controller->callReset();
			image = controller->kamera->readNext();
		}
		if(log != NULL) {
			log->debugStream() << "Snimok: " << image.pos_msec;
		}
		spracujJedenSnimok(image);
	} catch(ModulKamera::EndOfStream stream) {
		// Cyklus Run skonci, skonci apliakcia, spusti sa dekonstruktor, zacne sa uvolnovat pamet a vsetko vypinat ...
		if(log != NULL) {
			log->debugStream() << "Koniec streamu";
		}
		this->stop();
	} catch (std::out_of_range e) {
		if(log != NULL) {
			log->debugStream() << e.what();
		}
	} 
}
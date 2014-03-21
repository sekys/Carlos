/** @file stavy.cpp
* Trieda ktora sa stara o to co sa ma vykonat pri prepnuti stavu hry s logikou hry
*/ 
#include "class.Scene.hpp"
#include "..\..\..\com.carlos.architecture\db\class.DBService.hpp"
#include "..\help\class.ResourceManager.hpp"

using namespace DB;

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o restartovanie hry
* @see void Scene::delenieStavov(float fDelta)
* @return void 
*/
void Scene::stavSkore(FrameData* frame) {
	zasobnikVstupov.clear();

	///Ak sa dotkol displeja tak sa vrat do stavu hrania
	if(frame->getCommand() == ControllerCommands::UP) {
		prepniStavNaHrania();
	}
}
template <class T>
string convertToStr(T *var) {
	ostringstream ss;
	ss << *var;
	return ss.str();
}

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o prepnutie obrazovky po ubehnuti casu
* @see void Scene::delenieStavov(float fDelta)
* @see void Scene::prepniStavNaScore()
* @return void 
*/
void Scene::stavGameOver(FrameData* frame) {
	zasobnikVstupov.clear();
	casPrejdenyNaGameOver += frame->getDeltaTime();

	// Hud
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(-0.6f,-0.4f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.0f); 
	double d = duration;
	string str = convertToStr<double>(&d);
	const char * c = str.c_str();
	const char * result = "Aktualny cas letu: ";
	glPrint(result);
	glTranslatef(-0.1f,-0.4f,0.0f);
	glPrint(c);
	glEnable(GL_LIGHTING);


	if(casPrejdenyNaGameOver > 5.0) {
		/// presiel cas a zmeni sa stav
		prepniStavNaScore() ;
	}
}

void Scene::nastavPozadieZoVstupu(cv::Mat& img) {
	if(log != NULL) {
		log->debugStream() << "Nastavujem texturu z videa.";
	}
	GLenum inputColourFormat = GL_BGR;
	if (img.channels() == 1) {
		inputColourFormat = GL_LUMINANCE;
	}

	// 2. Prerob unsigned char na texturu
	CTexture texture;
	texture.createFromData(img.ptr(), img.cols, img.rows, GL_RGB, inputColourFormat, true);
	texture.setFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_NEAREST_MIPMAP);
	texture.flagDelete = true;

	// 3. nastav texturu na pozadie kocky
	setBackgroud(texture);
}

bool otestujHorizontCiSaDotykaLietadla(cv::Mat horizont, Plain* plain) {
	glm::vec2 aktualnaPozicia = plain->getPosition();
	glm::vec2 velkostLiedatla = plain->getsize();

	int os_min_x = aktualnaPozicia.x - velkostLiedatla.x/2 + 320;
	int os_max_x = aktualnaPozicia.x + velkostLiedatla.x/2 + 320;
	int os_min_y = aktualnaPozicia.y - velkostLiedatla.y/2 + 240;
	int os_max_y = aktualnaPozicia.y + velkostLiedatla.y/2 + 240;

	if (os_min_x < 0) return false; 
	if (os_max_x > 640) return false; 
	if (os_min_y < 0) return false; 
	if (os_max_y > 480) return false; 

	for (int j = os_min_y; j < os_max_y; j++ ){
		for (int i = os_min_x; i < os_max_x; i++ ){
			if(horizont.at<uchar>(j,i) != 0) return true;
		}
	}

	return false;
}

/** 
* Funkcia ma na vstupe jeden parameter, zobrazuje pozadie a overuje ci je lietadlo na obrazku
* @param fDelta - zmena casu
* @see void Scene::delenieStavov(float fDelta)
* @see void Scene::prepniStavNaHrania()
* @return void 
*/
void Scene::stavHrania(FrameData* frame) {
	if(!frame->hasVstup()) {
		if(log != NULL) {
			log->debugStream() << "Neprisiel mi snimok z videa, preskakujem nastavenie textury.";
		}
	} else {
		/// Kazdu snimku updatni pozadie
		nastavPozadieZoVstupu(frame->getImage());
	}
	/// Spustame logiku casti
	collisionStatus colStatus;
	colStatus = plain->collisionTest(*world); /// prava strana lietadla je 1 ked je na pravo colStatus.right
	// 001011
	//printf("%d %d %d %d\n",  colStatus.bottom, colStatus.left, colStatus.right, colStatus.top);
	// lietadlo je na lavej strane sveta a 1 je		right
	// lietadlo je na pravej strane a 1 je		left		a right
	// lietadlo je hore 1 je:					bottom		a right
	// lietadlo je dole							top		
	// v strede je								front, right, top 

	bool contain = world->contains(*plain);
	if(contain) {
		// cout << "Y\n"; // toto vracia Y aj ked sa len dotykaju, neskor sa to prepne na N
	} else {
		// cout <<"N\n";
		havaroval();
	}
	contain = plain->contains(*world);
	if(contain) {
		//cout << "A\n";
	} else {
		//cout << "B\n";  // toto vracia stale B
	}

	// Otestuj ci sa dotyka horizontu
	if(frame->hasVstup()) {

		cv::Mat horizont = frame->getHorizont();
		contain = otestujHorizontCiSaDotykaLietadla(horizont, plain);
		/*if(contain) {
		cout << "Narazil do horizontu\n";
		havaroval();
		} else {
		// toto vracia stale B
		//cout << "Leti nad horizontom\n";
		}*/
	}

	plain->logic(frame->getDeltaTime(), frame->getCommand() );
	visualController->renderObject(resManager->plain, plain->getMatrix());

	// Hud
	/*glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(0.0f,0.8f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.f); 
	//glPrint("STLAC MEDZERNIK");
	glEnable(GL_LIGHTING);*/
}
/** 
* Funkcia nema na vstupe ziaden parameter, stara sa o hru ze hrac vyletel mimo obrazovky
* @see void Scene::stavHrania(float fDelta)
* @return void 
*/
void Scene::havaroval() {
	prepniStavNaGameOver();
}


/** 
* Funkcia nema na vstupe ziaden parameter, stara sa o nastavenie uvodnej obrazovky
* @see void Scene::delenieStavov(float fDelta)
* @return void 
*/
void Scene::stavUvodnaObrazovka(FrameData* frame) {
	zasobnikVstupov.clear();


	///Ak sa dotkne obrazovky zacina sa hra
	if(frame->getCommand() == ControllerCommands::UP) {
		prepniStavNaHrania();
	}
}

void Scene::stateTouristInfo(FrameData *frame) {
	if (frame->hasVstup()) {
		nastavPozadieZoVstupu(frame->getImage());
		ModulVykreslovania::In *in = frame->getVstup();
		vector<ModulVypocitaniaPolohy::Out> najdeneObjekty = in->najdeneObjekty;

		if (najdeneObjekty.size() > 0) {
			// Vykreslime informacie o objektoch  databazy
			for (int i = 0; i < najdeneObjekty.size() && najdeneObjekty.at(i).najdeny; i++) {
				uint id = najdeneObjekty.at(i).id; 
				DB::Object *object;

				if (!objectInfos.count(id)) {
					object = DB::DBService::getInstance().getObjectById(id);

					if (object != NULL) {
						objectInfos[id] = object;
					}
				} else {
					object = objectInfos[id];
				}

				if (object != NULL) {
					showTouristInfo(object, najdeneObjekty.at(i).polohaTextu);
				}
			}
		} else {
			plain->setLastCommand(ControllerCommands::NO_ACTION);
		}
	}

	/*plain->setLastCommand(ControllerCommands::WHAT_IS_OBJECT);
	//Object *object = DB::DBService::getInstance().getObjectById(4);
	Object *object = new Object();
	object->name = "Testovacie menoTestovacie meno";
	showTouristInfo(object, Point2f(599, 449));*/
}

void Scene::showTouristInfo(DB::Object *object, Point2f &pos) {
	const char *str;

	if (plain->getLastCommand() == ControllerCommands::WHAT_IS_OBJECT) {
		str = object->name.c_str();
	} else if (plain->getLastCommand() == ControllerCommands::MORE_ABOUT_OBJECT) {
		str = object->short_description.c_str();
	} else {
		return;
	}

	vector<string> lines;
	unsigned int maxCharsOnLine = 0;
	formatter.formatTextToLines(str, lines);

	for (int i=0; i<lines.size(); i++) {
		if (lines[i].length() > maxCharsOnLine) {
			maxCharsOnLine = lines[i].length();
		}
	}

	for (int i=0; i<lines.size(); i++) {
		Point2f newPos;
		formatter.formatPosition(pos, newPos, i, lines.size(), 
			maxCharsOnLine, getWindowWidth(), getWindowHeight());
		printLineOfText(lines[i].c_str(), newPos.x, newPos.y);
	}
}

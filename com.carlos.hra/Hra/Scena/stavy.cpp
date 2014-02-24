/** @file stavy.cpp
* Trieda ktora sa stara o to co sa ma vykonat pri prepnuti stavu hry s logikou hry
*/ 
#include "class.Scene.hpp"
/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o restartovanie hry
* @see void Scene::delenieStavov(float fDelta)
* @return void 
*/
void Scene::stavSkore(FrameData* frame) {
	zasobnikVstupov.clear();

	///Ak sa dotkol displeja tak sa vrat do stavu hrania
	if(frame->command == ControllerCommands::UP) {
		prepniStavNaHrania();
	}
}

/** 
* Funkcia nema na vstupe ziadny parameter, stara sa o prepnutie obrazovky po ubehnuti casu
* @see void Scene::delenieStavov(float fDelta)
* @see void Scene::prepniStavNaScore()
* @return void 
*/
void Scene::stavGameOver(FrameData* frame) {
	zasobnikVstupov.clear();
	casPrejdenyNaGameOver += frame->deltaTime;
	if(casPrejdenyNaGameOver > 3.0) {
		/// presiel cas a zmeni sa stav
		prepniStavNaScore() ;
	}
}

void Scene::nastavPozadieZoVstupu(FrameData* frame) {
	printf("Nastavujem texturu z videa.\n");
	cv::Mat img = frame->vstup.image.data;

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
	if(frame->hasVstup == false) {
		printf("Neprisiel mi snimok z videa, preskakujem nastavenie textury.\n");
	} else {
		/// Kazdu snimku updatni pozadie
		nastavPozadieZoVstupu(frame);

		/// Spustame logiku casti
		collisionStatus colStatus;
		colStatus = plain->collisionTest(*world); /// prava strana lietadla je 1 ked je na pravo colStatus.right
		// 001011
		printf("%d %d %d %d\n",  colStatus.bottom, colStatus.left, colStatus.right, colStatus.top);
		// lietadlo je na lavej strane sveta a 1 je		right
		// lietadlo je na pravej strane a 1 je		left		a right
		// lietadlo je hore 1 je:					bottom		a right
		// lietadlo je dole							top		
		// v strede je								front, right, top 

		bool contain = world->contains(*plain);
		if(contain) {
			printf("Y\n"); // toto vracia Y aj ked sa len dotykaju, neskor sa to prepne na N
		} else {
			printf("N\n");
			havaroval();
		}
		contain = plain->contains(*world);
		if(contain) {
			printf("A\n");
		} else {
			printf("B\n");  // toto vracia stale B
		}

		// Otestuj ci sa dotyka horizontu
		cv::Mat horizont = frame->vstup.horizont;
		contain = otestujHorizontCiSaDotykaLietadla(horizont, plain);
		if(contain) {

			printf("Narazil do horizontu\n");
			havaroval();
		} else {
			// toto vracia stale B

			printf("Leti nad horizontom\n");

		}

		plain->logic(frame->deltaTime, frame->command);
		visualController.renderObject(resManager.plain, plain->getMatrix());
	}

	// Hud
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(0.0f,0.8f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.f); 
	//glPrint("STLAC MEDZERNIK");
	glEnable(GL_LIGHTING);
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

	// Hud
	glUseProgram(0);
	glDisable(GL_LIGHTING);
	glLoadIdentity();
	glTranslatef(0.0f,0.8f,0.0f);
	glColor3f(0.0f, 0.f, 0.f);
	glRasterPos2f(0.0f, 0.f); 
	//glPrint("STLAC MEDZERNIK");
	glEnable(GL_LIGHTING);

	///Ak sa dotkne obrazovky zacina sa hra
	if(frame->command == ControllerCommands::UP) {
		prepniStavNaHrania();
	}
}
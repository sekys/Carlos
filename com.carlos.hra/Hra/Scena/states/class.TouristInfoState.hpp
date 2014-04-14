#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"
#include "..\..\..\..\com.carlos.architecture\db\class.DBService.hpp"

using namespace DB;

class TouristInfoState : public IGameState {
private:

	class RenderedObject {
	public:
		uint id;

		/* Pozicia objektu na obrazovke v pixeloch */
		Point2f position;	

		/* Cas, kedy bol objekt zdetegovany na obrazovke od zaciatku behu modulu */
		double detectionTime;

		/* Cas od zaciatku behu mobulu, kedy sa objekt zacal renderovat inak, ako 
		 * len informacnou ikonkou (pouzivatel stlacil prikaz) */
		double renderingStartTime;

		/* Prikaz, ktory pouzivatel stlacil, respektive nestlacil a povie nam 
		 * ako mame objekt renderovat na obrazovke */
		ControllerCommands command;

		RenderedObject(uint id, Point2f position, double detectionTime) {
			this->id = id;
			this->position = position;
			this->detectionTime = detectionTime;
			this->renderingStartTime = -1.0;
			this->command = ControllerCommands::NO_ACTION;
		}

		// Ak bol objekt renderevonay inak ako len informacnou ikonkou (pouzivatel stlacil prikaz)
		bool isRendered() {
			return this->renderingStartTime >= 0.0;
		}
	};

	vector<RenderedObject> renderedObjects;

	bool isTouristInfoCommand(ControllerCommands command) {
		return (
			command == ControllerCommands::MORE_ABOUT_OBJECT ||
			command == ControllerCommands::WHAT_IS_OBJECT
		);
	}

	void showTouristInfo(ControllerCommands command, const DB::Object *object, Point2f &pos) {
		const char *str;

		if (command == ControllerCommands::WHAT_IS_OBJECT) {
			str = object->name.c_str();
		} else if (command == ControllerCommands::MORE_ABOUT_OBJECT) {
			str = object->short_description.c_str();
		} else {
			mScene->mVisualController->renderTexture(mScene->mResManager->infoImage, pos.x,
				pos.y, mScene->getWindowWidth(), mScene->getWindowHeight());
			return;
		}

		vector<string> lines;

		mScene->mFormatter.formatTextToLines(str, lines);

		unsigned int maxCharsOnLine = mScene->mFormatter.countMaxCharsOnLine(lines);

		for (int i=0; i<lines.size(); i++) {
			Point2f newPos;
			mScene->mFormatter.formatPosition(pos, newPos, i, lines.size(), 
				maxCharsOnLine, mScene->getWindowWidth(), mScene->getWindowHeight());
			printLineOfText(lines[i].c_str(), newPos.x, newPos.y);
		}
	}

	void eraseRenderedObjectsOutOfTime() {
		for (int i=0; i<renderedObjects.size(); i++) {
			RenderedObject &renderedObject = renderedObjects[i];

			if (renderedObject.isRendered()) {
				if (this->getCasBehu()-renderedObject.renderingStartTime > 5.0) {
					renderedObjects.erase(renderedObjects.begin() + i);
					i--;
				}
			} else {
				if (this->getCasBehu()-renderedObject.detectionTime > 5.0) {
					renderedObjects.erase(renderedObjects.begin() + i);
					i--;
				}
			}
		}
	}

	void renderObjects() {
		for (int i = 0; i < renderedObjects.size(); i++) {
			uint id = renderedObjects[i].id;
			const DB::Object *object = DB::DBService::getInstance().getObjectById(id);

			if (object != NULL) {
				showTouristInfo(renderedObjects[i].command, object, renderedObjects[i].position);
			}
		}
	}

public:
	TouristInfoState() : IGameState(GameStates::TOURIST_INFO) {}

	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgTouristInfoIntro);
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {
		// Prepneme sa do hry z turistickeho infa
		if (frame->getCommand() == ControllerCommands::UP || frame->getCommand() == ControllerCommands::GAME) {
			mScene->mStates->switchTo(GameStates::UVODNA_OBRAZOVKA);
			return;
		}

		// Pockame 5 sekund do zacatia ukazovania turistickeho infa
		if (this->getCasBehu() < 5.0) {
			return;
		}

		// Vyrenderujeme obrazok loga turistickeho infa
		mScene->mVisualController->renderTexture(
			mScene->mResManager->touristInfoLogo, 0, 0, mScene->getWindowWidth(), mScene->getWindowHeight()
		);

		if (frame->hasVstup()) {
			ModulVykreslovania::In *in = frame->getVstup();
			vector<ModulVypocitaniaPolohy::Out> najdeneObjekty = in->najdeneObjekty;

			for (int i = 0; i < najdeneObjekty.size(); i++) {
				if (najdeneObjekty[i].najdeny) {
					uint id = najdeneObjekty[i].id;
					int j = 0;

					for (; j<renderedObjects.size() && id!=renderedObjects[j].id; j++);

					Point2f position = najdeneObjekty[i].polohaTextu;

					// Prepocitame poziciu textu na obrazovke do pixelov
					position.x = position.x * mScene->getWindowWidth();
					position.y = position.y * mScene->getWindowHeight();

					if (j == renderedObjects.size()) { // Objekt je zdetegovany
						renderedObjects.push_back(RenderedObject(id, position, this->getCasBehu()));
					} else { // Objekt uz bol zdetegovany
						RenderedObject &renderedObject = renderedObjects[j];

						// Aktualizujeme poziciu
						renderedObject.position = position;

						// Aktualizujeme bud cas detekcie alebo cas renderovania, aby sa objekt neprestaval ukazovat na obrazovke
						if (renderedObject.isRendered()) {
							renderedObject.renderingStartTime = this->getCasBehu();
						} else {
							renderedObject.detectionTime = this->getCasBehu();
						}

						// Ak sme dostali prikaz z mobilu
						if (renderedObject.command == ControllerCommands::NO_ACTION &&
								isTouristInfoCommand(frame->getCommand())) {

							renderedObject.command = frame->getCommand();
							renderedObject.renderingStartTime = this->getCasBehu();
						}
					}
				}
			}
		} 

		mScene->setBlackBackground();

		eraseRenderedObjectsOutOfTime();

		renderObjects();

		/*plain->setLastCommand(ControllerCommands::WHAT_IS_OBJECT);
		//Object *object = DB::DBService::getInstance().getObjectById(4);
		Object *object = new Object();
		object->name = "Testovacie menoTestovacie meno";
		showTouristInfo(object, Point2f(599, 449));
		visualController->renderTexture(resManager->infoImage, 0, 0, getWindowWidth(), getWindowHeight());*/
	}
};
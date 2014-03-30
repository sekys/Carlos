#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"
#include "..\..\..\..\com.carlos.architecture\db\class.DBService.hpp"

using namespace DB;

class TouristInfoState : public IGameState {
private:
	void showTouristInfo(ControllerCommands command, DB::Object *object, Point2f &pos) {
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

public:
	TouristInfoState() : IGameState(GameStates::TOURIST_INFO) { 

	}

	virtual void switchOn(IGameState* predchodca) {
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {	
		if (frame->hasVstup()) {
			mScene->nastavPozadieZoVstupu(frame->getImage());
			ModulVykreslovania::In *in = frame->getVstup();
			vector<ModulVypocitaniaPolohy::Out> najdeneObjekty = in->najdeneObjekty;

			if (najdeneObjekty.size() > 0) {
				// Vykreslime informacie o objektoch  databazy
				for (int i = 0; i < najdeneObjekty.size(); i++) {
					if (najdeneObjekty.at(i).najdeny) {
						uint id = najdeneObjekty.at(i).id; 
						DB::Object *object = DB::DBService::getInstance().getObjectById(id);

						if (object != NULL) {
							showTouristInfo(frame->getCommand(), object, najdeneObjekty.at(i).polohaTextu);
						}
					}
				}
			} /*else {
			  plain->setLastCommand(ControllerCommands::NO_ACTION);
			  }*/
		}

		/*plain->setLastCommand(ControllerCommands::WHAT_IS_OBJECT);
		//Object *object = DB::DBService::getInstance().getObjectById(4);
		Object *object = new Object();
		object->name = "Testovacie menoTestovacie meno";
		showTouristInfo(object, Point2f(599, 449));
		visualController->renderTexture(resManager->infoImage, 0, 0, getWindowWidth(), getWindowHeight());*/

	}
};
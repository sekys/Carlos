#pragma once

#include "class.IGameState.hpp"
#include "../class.Scene.hpp"
#include "..\..\..\..\com.carlos.architecture\db\class.DBService.hpp"

using namespace DB;

class TouristInfoState : public IGameState {
private:

	class ObjectPostion {
	public:
		uint id;
		Point2f position;

		ObjectPostion(uint id, Point2f position) {
			this->id = id;
			this->position = position;
		}
	};

	vector<ObjectPostion> positions;

	bool isTouristInfoCommand(ControllerCommands command) {
		return (
			command == ControllerCommands::MORE_ABOUT_OBJECT ||
			command == ControllerCommands::WHAT_IS_OBJECT
		);
	}

	ControllerCommands lastCommand;

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

public:
	TouristInfoState() : IGameState(GameStates::TOURIST_INFO) {
		lastCommand = ControllerCommands::NO_ACTION;
	}

	virtual void switchOn(IGameState* predchodca) {
		mScene->setBackgroud(mScene->mResManager->bgTouristInfoIntro);
		mScene->zasobnikVstupov.clear();
	}

	virtual void frame(FrameData* frame) {
		// Prepneme sa do hry z turistickeho infa
		if (frame->getCommand() == ControllerCommands::GAME) {
			mScene->mStates->switchTo(GameStates::UVODNA_OBRAZOVKA);
			return;
		}

		// Pockame 5 sekund do zacatia ukazovania turistickeho infa
		if (this->getCasBehu() < 5.0) {
			return;
		}

		ModulVykreslovania::In *in = frame->getVstup();

		if (frame->hasVstup()) {
			positions.clear();

			vector<ModulVypocitaniaPolohy::Out> najdeneObjekty = in->najdeneObjekty;

			if (!najdeneObjekty.empty()) {
				if (isTouristInfoCommand(frame->getCommand()) ||
					isTouristInfoCommand(lastCommand)) {

					for (int i = 0; i < najdeneObjekty.size(); i++) {
						if (najdeneObjekty.at(i).najdeny) {
							uint id = najdeneObjekty.at(i).id; 
							Point2f position = najdeneObjekty.at(i).polohaTextu;

							// Prepocitame poziciu textu na obrazovke do pixelov
							position.x = position.x * mScene->getWindowWidth();
							position.y = position.y * mScene->getWindowHeight();

							positions.push_back(ObjectPostion(id, position));
						}
					}

					lastCommand = frame->getCommand();
				}
			} else {
				lastCommand = ControllerCommands::NO_ACTION;
			}
		}

		mScene->setBlackBackground();

		for (int i = 0; i < positions.size(); i++) {
			uint id = positions[i].id;
			const DB::Object *object = DB::DBService::getInstance().getObjectById(id);

			if (object != NULL) {
				showTouristInfo(lastCommand, object, positions[i].position);
			}
		}

		/*plain->setLastCommand(ControllerCommands::WHAT_IS_OBJECT);
		//Object *object = DB::DBService::getInstance().getObjectById(4);
		Object *object = new Object();
		object->name = "Testovacie menoTestovacie meno";
		showTouristInfo(object, Point2f(599, 449));
		visualController->renderTexture(resManager->infoImage, 0, 0, getWindowWidth(), getWindowHeight());*/
	}
};
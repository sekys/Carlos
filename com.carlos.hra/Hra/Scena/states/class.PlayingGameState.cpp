#include "class.PlayingGameState.hpp"

bool PlayingGameState::otestujHorizontCiSaDotykaLietadla(cv::Mat horizont, Plane* plain, int pozicia) {
	cv::flip(horizont, horizont, 0);
	glm::vec2 aktualnaPozicia = plain->getPosition();
	glm::vec2 velkostLiedatla = plain->getsize();

	//int os_min_x = aktualnaPozicia.x - velkostLiedatla.x/2 + 320;
	//int os_max_x = aktualnaPozicia.x + velkostLiedatla.x/2 + 320;
	int os_min_y = aktualnaPozicia.y - velkostLiedatla.y/2 + 240; 
	int os_max_y = aktualnaPozicia.y + velkostLiedatla.y/2 + 240;

	//if (os_min_x < 0) return false; 
	//if (os_max_x > 640) return false; 
	if (os_min_y < 0 || os_min_y > 480) return false; 
	if (os_max_y > 480 || os_max_y < 0) return false; 

	for (int j = 465+pozicia; j < 475+pozicia; j++ ){
		for (int i = os_min_y; i < os_max_y; i++ ){
			if(horizont.at<uchar>(os_min_y,j) != 0) return true;
		}
	}

	return false;
}

void PlayingGameState::dokresliHorizont(cv::Mat& bg, cv::Mat& horizont) {
	for( int j= 0; j < bg.cols; j++ )
	{
		bg.at<Vec3b>(119,j)[0] = 0;
		bg.at<Vec3b>(119,j)[1] = 255;
		bg.at<Vec3b>(119,j)[2] = 0;
		for( int i = 120; i < bg.rows; i++ )
		{

			if(horizont.at<uchar>(i,j) == 0){
				bg.at<Vec3b>(i,j)[0] = 0;
				bg.at<Vec3b>(i,j)[1] = 0;
				bg.at<Vec3b>(i,j)[2] = 255;
				break;
			}
		}
	}
}

void PlayingGameState::eventHavaroval() {
	mScene->mStates->switchTo(GameStates::OBRAZOVKA_PREHRAL);
}

void PlayingGameState::switchOn(IGameState* predchodca) {
	mScene->mPlane->setStartPosition();
	mScene->zasobnikVstupov.clear();
}

void PlayingGameState::frame(FrameData* frame) {
	if (frame->getCommand() == ControllerCommands::TOURIST_INFO) {
		mScene->mStates->switchTo(GameStates::TOURIST_INFO);
		return;
	}

	if(!frame->hasVstup()) {
		if(log != NULL) {
			log->debugStream() << "Neprisiel mi snimok z videa, preskakujem nastavenie textury.";
		}
	} else {
		Architecture::ModulVykreslovania::In *in = frame->getVstup();
		mScene->aktualnaPozicia = frame->getpozicia();
		cv::Mat black(480, 640, CV_8UC3, Scalar(0,0,0));
		dokresliHorizont(black, in->horizont);
		cv::flip(black, black, 0);
		mScene->setBackgroud(black.clone());
	}

	bool contain = mScene->mWorld->contains(*mScene->mPlane);
	if(!contain) {
		this->eventHavaroval();
	}

	// Otestuj ci sa dotyka horizontu
	if(frame->hasVstup()) {
		cv::Mat horizont = frame->getHorizont();
		contain = otestujHorizontCiSaDotykaLietadla(horizont, mScene->mPlane, mScene->aktualnaPozicia);
		if(!contain) {
			cout << "Narazil do horizontu\n";
			this->eventHavaroval();
		}
	}

	mScene->mPlane->logic(frame->getDeltaTime(), frame->getCommand() );
	mScene->mVisualController->renderObject(mScene->mResManager->plane, mScene->mPlane->getMatrix(mScene->aktualnaPozicia+150));
}
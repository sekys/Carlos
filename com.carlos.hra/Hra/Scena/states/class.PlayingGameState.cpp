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
	int aktualnaPozicia = 0;

	if(!frame->hasVstup()) {
		if(log != NULL) {
			log->debugStream() << "Neprisiel mi snimok z videa, preskakujem nastavenie textury.";
		}
	} else {
		Architecture::ModulVykreslovania::In *in = frame->getVstup();
		mScene->aktualnaPozicia = frame->getpozicia();
		cv::Mat black(480, 640, CV_8UC3, Scalar(0,0,0));
		in->image.data = black; 
		in->horizont = in->horizont.clone();
		dokresliHorizont(in->image.data, in->horizont);
		cv::flip(in->image.data, in->image.data, 0);
		/// Kazdu snimku updatni pozadie
		mScene->nastavPozadieZoVstupu(frame->getImage());
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
	int miesto;

	

	mScene->mVisualController->renderObject(mScene->mResManager->plane, mScene->mPlane->getMatrix(mScene->aktualnaPozicia+150));
}


/// Spustame logiku casti
//collisionStatus colStatus;
//colStatus = mScene->mPlane->collisionTest(*(mScene->mWorld)); 
/// prava strana lietadla je 1 ked je na pravo colStatus.right
// 001011
//printf("%d %d %d %d\n",  colStatus.bottom, colStatus.left, colStatus.right, colStatus.top);
// lietadlo je na lavej strane sveta a 1 je		right
// lietadlo je na pravej strane a 1 je		left		a right
// lietadlo je hore 1 je:					bottom		a right
// lietadlo je dole							top		
// v strede je								front, right, top 

/*
bool contain = mScene->mWorld->contains(*mScene->mPlane);
if(contain) {
// cout << "Y\n"; // toto vracia Y aj ked sa len dotykaju, neskor sa to prepne na N
} else {
// cout <<"N\n";
this->eventHavaroval();
}
contain = plain->contains(*world);
if(contain) {
//cout << "A\n";
} else {
//cout << "B\n";  // toto vracia stale B
}

*/
#include <windows.h>
#include "../com.carlos.architecture/DllExports.h"
#include "../com.carlos.architecture/modules/class.ModulSpracovania.hpp"
#include "CV.h"
#include <iostream>
#ifdef _DEBUG
	#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#else
	#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#endif

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

void RealModulSpracovania::init() {
	const int th = 500; // 400
	detector.hessianThreshold = th;
	setParameters();
	if(!prevHorizon.empty()) prevHorizon.release();
	prevPos = NULL;
#ifdef GPU_MODE
	siftGpu = NULL;
#endif
}

/** detekcia objektov z mnoziny moznych objektov
*	@param in obsahuje mnozinu objektov spolu s ich cestami, ktore sa na vstupnom obrazku moozu nachadzat
*	@return najpravdepodobnejsi objekt na obrazku
*/

ModulSpracovania::Out RealModulSpracovania::detekujObjekty(ModulSpracovania::In in) {
	ModulSpracovania::Out out; /**< najdeny objekt */
	Mat scene; /**< vstupny obrazok */
	vector<Candidate> candidates; /**< kandidati na detekovany objekt */
	vector<KeyPoint> sceneKeyPoints; /**< keypointy streamu */
	Mat sceneDescriptors; /**< matica deskriptorov streamu */

	if( in.image.data.empty() || in.recepts.empty() ) return out; /// chyba nacitania streamu alebo ziadne mozne objekty
	cvtColor( in.image.data, scene, CV_BGR2GRAY );
	
#ifdef GPU_MODE
	runSiftGpu(scene, sceneKeyPoints, sceneDescriptors);
#else
	runSurf(scene, sceneKeyPoints, sceneDescriptors);  /// najdi keypoints a deskriptory
#endif

	int imported = importCandidates(candidates, in.recepts); /**< pocet kandidatov na objekty */
	if( imported == 0 ) return out; /// neuspesny import

	for(int i = 0; i < candidates.size(); i++ )
	{
		Mat H; /**< matica perspektivnej transformacie */
		vector<DMatch> matches; /**< zhody */
		robustMatching( candidates[i].descriptors, sceneDescriptors, matches, distanceRatioThreshold ); /// najdi dobre zhody
			
		candidates[i].valid = selectInliers( candidates[i].keyPoints, sceneKeyPoints, matches, H ); /// odstran outliers a ziskaj maticu perspektivnej transformacie
		if( candidates[i].valid == false ) continue; /// neuspesna detekcia objektu na vstupnom obrazku
		else
		{
			candidates[i].avgDistance = computeAvgDistance( matches ); /// priemerna L2 vzdialenost zhod
			candidates[i].confidence = computeConfidence( candidates[i].keyPoints.size(), sceneKeyPoints.size(), matches.size() ); /// pomer medzi bodmi a dobrymi zhodami (urcuje spravnost detekcie)
			candidates[i].valid = findObject( candidates[i].image, H, candidates[i].confidence, candidates[i].position1, candidates[i].position2 ); /// najdi poziciu objektu
		}
	}

 	int index = getBestCandidate( candidates ); /**< najlepsi kandidat za objekt */

	if( index >= 0 ) /// najdenie pozicie najlepsieho kandidata
	{
		DetekovanyObjekt najdenyObjekt;
		
		najdenyObjekt.objekt = in.recepts[ candidates[index].pos ]; /**< detegovany objekt */
		
		najdenyObjekt.boundary = RotatedRect(
			Point2f( candidates[index].position1.x + ( candidates[index].position2.x - candidates[index].position1.x ) /2, candidates[index].position1.y + ( candidates[index].position2.y - candidates[index].position1.y) / 2 ), 
			Size( candidates[index].position2.x - candidates[index].position1.x ,  candidates[index].position2.y - candidates[index].position1.y ), 
			0
			); /**< pozicia objektu */
		
		cout << "Najdeny objekt #" << index << endl;
	
		// Poslem vysledok dalej
		out.objects.push_back(najdenyObjekt); /**< vlozit vysledok */
	}
	
	return out;
}

/** inicializacia kandidata */

Candidate::Candidate():confidence(0.),avgDistance(0.),valid(true)
{
}

/** vyber najlepsieho kandidata na objekt
*	@param candidates potencialny kandidati na objekt
*	@return index najlepsieho kandidata
*/

int RealModulSpracovania::getBestCandidate(const vector<Candidate> &candidates)
{
	int index = -1;
	
	for(int i = 0; i < candidates.size(); i++)
	{
		if( candidates[i].valid && candidates[i].confidence >= 0 && candidates[i].avgDistance < maxAvgDistance
			&&  ( index < 0 || candidates[index].confidence < candidates[i].confidence ) )
			index = i;
	}

	return index;
}

/** vypocet priemernej L2 zo vsetkych matchov
*	@param matches zhody
*	@return priemerna L2 zo vsetkych matchov
*/

double RealModulSpracovania::computeAvgDistance(const vector<DMatch> &matches)
{
	double avgDistance = 0;

	if(matches.empty() ) return avgDistance;

	for( int i = 0; i < matches.size(); i++ )
	{
		avgDistance += matches[i].distance;
	}
	avgDistance /= matches.size();

	return avgDistance;
}

/** vypocet pravdepodobnosti ze sa jedna o hladany objekt
*	pravdepodobnost sa vypocita na zaklade pomeru poctu keypointov a a "dobrych" zhod
*	@param keypoints1_size pocet keypointov na jednom obrazku
*	@param keypoints2_size pocet keypointov na druhom obrazku
*	@param matches_size pocet zhod medzi keypointami
*	@return pravdepodobnost
*/

double RealModulSpracovania::computeConfidence(int keypoints1_size, int keypoints2_size, int matches_size)
{
	double confidence = 0;
	confidence = (double) matches_size / ( ( keypoints1_size + keypoints2_size )*0.5 );
	return confidence;
}

/** najdenie pozicie objektu na obrazku
*	@param image obrazok objektu
*	@param H matica perspektivnej transf.
*	@confidence pravd. ze sa jedna o objekt
*	@position1 lavy horny roh objektu
*	@position2 pravy dolny roh objektu
*	@return bolo hladanie uspesne?
*/

bool RealModulSpracovania::findObject(const Mat &image, const Mat &H, double confidence, Point2f &position1, Point2f &position2)
{
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); 
	obj_corners[1] = cvPoint( image.cols, 0 );
	obj_corners[2] = cvPoint( image.cols, image.rows ); 
	obj_corners[3] = cvPoint( 0, image.rows );
	vector<Point2f> scene_corners(4);
	Rect roi;
	const int minWidth = 10;
	const int minHeight = 10;

	if( confidence < minRecognitionConfidence ) return false;

	try{
		perspectiveTransform( obj_corners, scene_corners, H );
		roi = boundingRect( scene_corners );
		
		if(roi.width < minWidth || roi.height < minHeight ) return false; /// objekt musi splnat min. velkost

		position1 = Point2f( roi.x, roi.y );
		position2 = Point2f( roi.x + roi.width, roi.y + roi.height );
	}catch(Exception &e)
	{
		return false; /// neuspesna perspektivna transf.
	}

	return true;
}

/** pre obrazok z DB nacitaj keypoint-y a deskriptory zo suboru
*	@param path zlozka s obrazkom
*	@param id ID obrazku
*	@param keyPoints keypoint-y
*	@param descriptors deskriptory
*	@return uspesnost nacitania
*/
bool RealModulSpracovania::readDescriptors(const string path, const int id, vector<KeyPoint> &keyPoints, Mat &descriptors)
{
	char *fileFormat = "images\\%s\\image-%04d.yml"; /// cesta ku keypoint-om a deskriptorom

	FileStorage fs(format(fileFormat, path.c_str(), id), FileStorage::READ);
	if(!fs.isOpened()) return false;
	/// nacitaj keypoint-y
	FileNode kptFileNode = fs["keypoints"];
	read( kptFileNode, keyPoints );
	/// nacitaj deskriptory
	FileNode dscFileNode = fs["descriptors"];
	read( dscFileNode, descriptors );
	fs.release();

	if(keyPoints.empty() || descriptors.empty()) return false;

	return true;
}

/** pre obrazok z DB zapis keypoint-y a deskriptory do suboru
*	@param path zlozka s obrazkom
*	@param id ID obrazku
*	@param keyPoints keypoint-y
*	@param descriptors deskriptory
*	@return uspesnost zapisu
*/
bool RealModulSpracovania::writeDescriptors(const string path, const int id, vector<KeyPoint> keyPoints, Mat descriptors)
{
	char *fileFormat = "images\\%s\\image-%04d.yml"; /// cesta ku keypoint-om a deskriptorom

	if(keyPoints.empty() || descriptors.empty()) return false;
	
	FileStorage fs(format(fileFormat, path.c_str(), id), FileStorage::WRITE);
	/// uloz keypoint-y
	fs << "keypoints" << keyPoints; 
	/// uloz deskriptory
	fs << "descriptors" << descriptors;
	fs.release();
	
	return false;
}

/** z moznych objektov kt. mozu byt na obrazku vytvor kandidatov
*	@param candidates mnozina kandidatov
*	@param objects mnozina potencialnych objektov
*	@return pocet kandidatov
*/

int RealModulSpracovania::importCandidates(vector<Candidate> &candidates, vector<WorldObject> &objects) // nacitaj pre vsetky mozne objekty vsetky obrazky a najdi keypoints a maticu deskriptorov 
{
	int count = 0; /// pocet nacitanych kandidatov
	char *fileFormat = "images\\%s\\image-%04d.jpg"; /// zatial bude cesta k suborom takato
	WorldObject object;
	
	for (int pos = 0; pos < objects.size(); pos++)
	{
		object = objects.at(pos);
		while(true)
		{
			Candidate candidate;
			candidate.id = object.id;
			candidate.pos = pos;
			string path = format(fileFormat, object.cestyKSuborom[0].c_str(), count+1);
			candidate.image = imread(path, IMREAD_GRAYSCALE); /// nacitaj obrazky
			if(candidate.image.empty()) break;
#ifdef GPU_MODE
			runSiftGpu(candidate.image, candidate.keyPoints, candidate.descriptors);
			if(candidate.keyPoints.empty() || candidate.keyPoints.size() < minKeypointsPerObject || candidate.descriptors.empty()) continue;
			candidates.push_back(candidate);
			count++;
#else
			// runSurf(candidate.image, candidate.keyPoints, candidate.descriptors); 
			/// pokus sa nacitat keypoint-y a deskriptory zo suboru, inak ich prepocitaj nanovo a zapis do suboru
			if(!readDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors))
			{	
				runSurf(candidate.image, candidate.keyPoints, candidate.descriptors); /// najdi keypoint-y a deskriptory
				writeDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors);
			}
			if(candidate.keyPoints.empty() || candidate.keyPoints.size() < minKeypointsPerObject || candidate.descriptors.empty()) continue;
			candidates.push_back(candidate);
			count++;
#endif
		}
	}

	return count;
}

/** vytvor maticu perspektivnej transf. 
*	volitelne: zo vsetkych matchov vyber len tie spravne zhody - inliers
*	@param objectKeypoints keypointy objektu
*	@param sceneKeypoints keypointy sceny (vstupneho obrazu)
*	@param matches zhody
*	@param H matica perspektivnej transf.
*	@return uspesne vytvorenie H?
*/

bool RealModulSpracovania::selectInliers(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H)
{
	vector<Point2f> obj_points;
	vector<Point2f> scene_points;
	vector<uchar> inliers(matches.size(), 0);

	if(sceneKeypoints.size() > minKeypointsPerObject * 2 && matches.size() > minMatchesToFindHomography )
	{
		for( int x = 0; x < matches.size(); x++ )
		{
			/// Ziskaj keypointy z "dobrych zhod"
			obj_points.push_back( objectKeypoints[ matches[x].queryIdx ].pt );
			scene_points.push_back( sceneKeypoints[ matches[x].trainIdx ].pt );
		}

		try{
			H = findHomography( obj_points, scene_points, inliers, CV_RANSAC ); /// najdi homografiu - maticu perspektivnej transformacie
					
			if(ransacOutliersRemovalEnabled)
				extractInlierMatches(matches, inliers);

			/*objects[ii].confidence = (double) objects[ii].matches.size() / ( ( objects[ii].keyPoints.size() + sceneKeyPoints.size() )*0.5 );
			if( objects[ii].confidence > minRecognitionConfidence )
			{
				objects[ii].H = H;
					
				vector<Point2f> obj_corners(4);
				obj_corners[0] = cvPoint(0,0); 
				obj_corners[1] = cvPoint( objects[ii].image.cols, 0 );
				obj_corners[2] = cvPoint( objects[ii].image.cols, objects[ii].image.rows ); 
				obj_corners[3] = cvPoint( 0, objects[ii].image.rows );
						
				std::vector<Point2f> scene_corners(4);
				perspectiveTransform( obj_corners, scene_corners, objects[ii].H );
				objects[ii].scene_corners = scene_corners;
			}*/
		}catch(Exception &e)
		{
			// cerr << e.what() << endl;
			return false;
		}
	}
	else return false;

	if( matches.size() < minMatchesToFindHomography ) return false;

	return true;
}

/** filtrovanie zhod
*	dobra zhoda je len taka ak sa najde oboma sposobmi: 1.obrazok -> 2.obrazok a 2.obrazok -> 1.obrazok
*	@param matches12 zhody - 1.obrazok -> 2.obrazok
*	@param matches21 zhody - 2.obrazok -> 1.obrazok
*	@param matches vyfiltrovane zhody
*/

void RealModulSpracovania::crossCheckFilter(vector<vector<DMatch>> matches12, vector<vector<DMatch>> matches21, vector<DMatch> &matches)
{
	DMatch match;
	
	matches.clear();
	int size = maximum(matches12.size(), matches21.size());

	for(int i = 0; i < matches12.size() ; i++)
	{
		if(matches12[i].size() < 2) continue;

		match = matches12[i][0];
		

		if( !( matches21[match.trainIdx].size() < 2 ) && ( matches21[match.trainIdx][0].trainIdx == match.queryIdx ) )
			matches.push_back(match);
	}

}

/** pre kazdy keypoint najdi k najpravdepodobnejsich zhod oboma smermi
*	@param descriptors1 matica deskriptorov 1.obrazku
*	@param descriptors2 matica deskriptorov 2.obrazku
*	@param matches12 zhody - 1.obrazok -> 2.obrazok
*	@param matches21 zhody - 2.obrazok -> 1.obrazok
*	@param k pocet zhod pre kazdy keypoint
*/

void RealModulSpracovania::crossKnnMatch(Mat descriptors1, Mat descriptors2, vector<vector<DMatch>> &matches12, vector<vector<DMatch>> &matches21, int k)
{
	matches12.clear();
	matches21.clear();

	if(descriptors1.empty() || descriptors2.empty()) return;

	matcher.knnMatch(descriptors1,descriptors2,matches12, k); /// najdi pre kazdy keypoint k najpravdepodobnejsich zhod 
	matcher.knnMatch(descriptors2,descriptors1,matches21, k); /// najdi pre kazdy keypoint k najpravdepodobnejsich zhod 
}

/** vyfiltruj zhody, ktorych vzdialenost najlepsieho paru dostatocne mensia ako vzdialenost 2. najlepsieho paru
* @param matches zhody
* @param th threshold medzi parmi
*/

void RealModulSpracovania::distanceRatioFilter(vector<vector<DMatch>> &matches, double th)
{
	for( int i = 0; i < matches.size(); i++ )
	{ 
		if( matches[i].size() == 2 && matches[i][1].distance > 0 && matches[i][0].distance > th*matches[i][1].distance )
			matches[i].clear();
	}
}

/** zo zhod vyselektuj len spravne zhody na zaklade inliers
* @param matches zhody
* @param inliers spravne zhody
*/

void RealModulSpracovania::extractInlierMatches(vector<DMatch> &matches, vector<uchar> inliers)
{
	int lastBadMatchIndex = -1;
	
	for (int i = 0; i < matches.size(); ++i) {
		if (inliers[i]) {
			if (lastBadMatchIndex < 0)
				continue;
			matches[lastBadMatchIndex] = matches[i];
			++lastBadMatchIndex;
		}
		else if (lastBadMatchIndex < 0)
			lastBadMatchIndex = i;
	}

	if (lastBadMatchIndex < 0)
		return;
	matches.resize(lastBadMatchIndex);
}

/** nastav parametre pre vsetky threshooldy */

void RealModulSpracovania::setParameters(double distanceRatioThreshold, int minMatchesToFindHomography, bool ransacOutliersRemovalEnabled, int minKeypointsPerObject, double minRecognitionConfidence, double maxAvgDistance)
{
	this->distanceRatioThreshold = distanceRatioThreshold, 
	this->minMatchesToFindHomography = minMatchesToFindHomography,
	this->ransacOutliersRemovalEnabled = ransacOutliersRemovalEnabled,
	this->minKeypointsPerObject = minKeypointsPerObject,
	this->minRecognitionConfidence = minRecognitionConfidence;
	this->maxAvgDistance = maxAvgDistance;
}

/** matchovanie keypointov len s vyuzitim "vzdialenostneho filtra" @see RealModulSpracovania::distanceRatioFilter
*	@param descriptors1 matica deskriptorov 1. obrazka
*	@param descriptors2 matica deskriptorov 2. obrazka
*	@param matches "dobre zhody"
*	@param minRatioThreshold min. vzdialenost
*/

void RealModulSpracovania::distanceMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12;

	matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
	distanceRatioFilter(matches12, minRatioThreshold);

	for(int i = 0; i < matches12.size(); i++)
		if(matches12[i].size() > 0) matches.push_back( matches12[i][0] );
}

/** klasicke matchovanie keypointov bez filtra
*	@param descriptors1 matica deskriptorov 1. obrazka
*	@param descriptors2 matica deskriptorov 2. obrazka
*	@param matches "dobre zhody"
*/

void RealModulSpracovania::simpleMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches)
{
	matcher.match(descriptors1, descriptors2, matches);
}

/** robustne matchovanie keypointov s vyuzitim 
*	1. "vzdialenostneho filtra" @see RealModulSpracovania::distanceRatioFilter
*	1. "krizoveho filtra" @see RealModulSpracovania::crossCheckFilter
*	@param descriptors1 matica deskriptorov 1. obrazka
*	@param descriptors2 matica deskriptorov 2. obrazka
*	@param matches "dobre zhody"
*	@param minRatioThreshold min. vzdialenost
*/

void RealModulSpracovania::robustMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12, matches21;
	matches.clear();

	crossKnnMatch(descriptors1, descriptors2, matches12, matches21, 2);

	distanceRatioFilter(matches12, minRatioThreshold);
	distanceRatioFilter(matches21, minRatioThreshold);
	crossCheckFilter(matches12, matches21, matches);
}

/** detegovat keypointy a opisat ich pomocou SURF deskriptorov
*	@param image obrazok
*	@param keypoints keypointy
*	@param descriptors matica deskriptorov
*/

void RealModulSpracovania::runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	detector.detect(image, keypoints);
	//if( keypoints.size() > 3000 ) keypoints.resize(3000);
	extractor.compute(image, keypoints, descriptors);
}

#ifdef GPU_MODE
void RealModulSpracovania::runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	char *argv[] = {"-v", "1",  "-p", "640x480"};
	int argc = sizeof(argv) / sizeof(char *);
	siftGpu = new SiftGPU();
	siftGpu->ParseParam(argc, argv);

	//	if (siftGpu->VerifyContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED) {
	if (siftGpu->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED)
	{
		siftGpu = NULL;
		return;
	}

	//	}
	if (!siftGpu->RunSIFT(image.cols, image.rows, image.data, GL_LUMINANCE, GL_UNSIGNED_BYTE)) {
		keypoints.resize(0);
		descriptors = Mat();
		return;
	}

	const int numFeatures = siftGpu->GetFeatureNum();
	SiftGPU::SiftKeypoint *siftKeypoints = new SiftGPU::SiftKeypoint[numFeatures];
	descriptors.create(numFeatures, 128, CV_32FC1);
	siftGpu->GetFeatureVector(siftKeypoints, (float *) descriptors.data);

	keypoints.resize(0);
	keypoints.reserve(numFeatures);

	KeyPoint keypoint;
	Point2f *point;

	for (int i = 0; i < numFeatures; ++i) {
		point = (Point2f *) &siftKeypoints[i];

		keypoint.pt = *point;
		//keypoint.size = siftKeypoints[i].s;
		//keypoint.angle = siftKeypoints[i].o;
		keypoints.push_back(keypoint);
	}

	delete [] siftKeypoints;
	delete siftGpu;
}
#endif

/** kalibracia kamery na zaklade 2 vstupnych obrazkov
*	najdenie prespektivnej transformacie medzi nimi
*	@param image1 zdeformovany obraz
*	@param image2 spravny obraz
*	@return matica perspektivnej transf.
*/

Mat RealModulSpracovania::kalibruj(Mat image1, Mat image2) {
	Mat out;

	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;

	if( image1.empty() || image2.empty() ) return out; /// chyba nacitania streamu alebo ziadne mozne objekty
	cvtColor( image1, image1, CV_BGR2GRAY );
	cvtColor( image2, image2, CV_BGR2GRAY );

#ifdef GPU_MODE
	runSiftGpu(scene, sceneKeyPoints, sceneDescriptors);
#else
	runSurf(image1, keypoints1, descriptors1);  /// najdi keypoints a deskriptory
	runSurf(image2, keypoints2, descriptors2);  /// najdi keypoints a deskriptory
#endif

	Mat H;
	vector<DMatch> matches;
	robustMatching( descriptors1, descriptors2, matches, distanceRatioThreshold ); /// najdi dobre zhody
			
	if(findMatrix( keypoints1, keypoints2, matches, H ) == false) return out; /// odstran outliers a ziskaj maticu perspektivnej transformacie
	
	out= H;
	return out;
}

/** najdi maticu perspektivnej transf.
*	@param objectKeypoints keypointy 1. obrazku
*	@param sceneKeypoints keypointy 2. obrazku
*	@param matches zhody
*	@param H matica perspektivnej transformacie
*	@return uspesne hladanie?
*/

bool RealModulSpracovania::findMatrix(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H)
{
	vector<Point2f> obj_points;
	vector<Point2f> scene_points;
	vector<uchar> inliers(matches.size(), 0);

	if(sceneKeypoints.size() > minKeypointsPerObject * 2 && matches.size() > minMatchesToFindHomography )
	{
		for( int x = 0; x < matches.size(); x++ )
		{
			obj_points.push_back( objectKeypoints[ matches[x].queryIdx ].pt );
			scene_points.push_back( sceneKeypoints[ matches[x].trainIdx ].pt );
		}

		try{
			H = findHomography( obj_points, scene_points, inliers, CV_RANSAC ); /// najdi homografiu - maticu perspektivnej transformacie
		}catch(Exception &e){
			return false;
		}
	}
	else return false;

	if( matches.size() < minMatchesToFindHomography ) return false;

	return true;
}

/** nadi horizont na obrazku
*	@param image vstupny obrazok
*	@return bitmapa s vysegmentovanou oblohou (biela farba)
*/

Mat RealModulSpracovania::najdiHorizont(Mat image)
{
	Mat horizon;
	cvtColor( image, image, CV_BGR2GRAY );
	cannyHorizonDetection(image, horizon);
	return horizon;
}

/** detekcia horizontu s vyuzitim hranoveho detektoru Canny
*	@param image vstupny obrazok
*	@param horizon obrazok s najdenou oblohou
*/

void RealModulSpracovania::cannyHorizonDetection(const Mat &image, Mat &horizon)
{
	const double ratio = 3;
	const int kernel_size = 3;
	const int lowThreshold = 130;
	const int delta = 50;
	const int beta = 15;
	const int increment = 3;
	const int roi_width = 50;
	const int minSky = 120;
	const int color_white = 255;
	const int color_black = 0;
	const int maxPxDiff = 1500;
	int *pos = new int[image.cols];

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(13, 13) );

	horizon.create( image.size(), CV_8UC1 );

	/// Canny detector
	Canny( image, horizon, lowThreshold, lowThreshold*ratio, kernel_size );

	/// kreslenie oblohy po pasoch zhora nadol
	for( int j = 0; j < horizon.cols; j++ )
	{
		int color = color_white;
		pos[j] = minSky;
		for( int i = 0; i < horizon.rows; i++ )
		{
			if(horizon.at<uchar>(i,j) != 0 && color == color_white) 
			{
				color = color_black;
				if( i > minSky) pos[j] = i;
			}
		}
		if(color == color_white ) pos[j] = 0;
	}

	if( !prevHorizon.empty() )
	{
		Mat tmp;
		int nonZero;
		bitwise_xor( prevHorizon, horizon, tmp );
		nonZero = countNonZero( tmp );
		cout << "Total number: " << nonZero << endl; 
		if(nonZero > maxPxDiff)
		{
			for(int i = 0; i < image.cols; i++)
			{
				if( prevPos[i] - pos[i] > delta )
				{
					int start = 0, end = image.cols - 1, new_height = 0;
					if( i - roi_width / 2 > -1 ) start = i - roi_width / 2;
					if( i + roi_width / 2 < image.cols ) end = i + roi_width / 2;
					for( int j = start; j < end; j++ )
						new_height += prevPos[j];

					new_height /= end - start;
					if( new_height < minSky ) new_height = minSky;
					else if( new_height >= image.rows )  new_height = image.rows - 1;
					pos[i] = new_height;
				}
			}
		}
	}

	for(int i = 1; i < image.cols; i++)
	{
		if( pos[i-1] - pos[i] > delta*2 )
		{
			if( pos[i-1] > pos[i] )
			{
				pos[i] = pos[i-1] - increment;
			}
			for(int j = i + 1; j < i + 200; j++)
			{
				if(j >= image.cols) break;
				if( abs( pos[j] - pos[i] ) < beta)
				{
					pos[i] = pos[j];
				}
			}
		}
		else
		{
			int start = 0, end = image.cols - 1, new_height = 0;
			if( i - roi_width / 2 > -1 ) start = i - roi_width / 2;
			if( i + roi_width / 2 < image.cols ) end = i + roi_width / 2;
			for( int j = start; j < end; j++ )
				new_height += pos[j];

			new_height /= end - start;
			if( new_height < minSky ) new_height = minSky;
			else if( new_height >= image.rows )  new_height = image.rows - 1;
			pos[i] = new_height;
		}
	}

	horizon.create( image.size(), CV_8UC1 );

	horizon = Scalar::all(255);

	for( int j = 0; j < horizon.cols; j++ )
	{
		for( int i = pos[j]; i < horizon.rows; i++ )
		{
			horizon.at<uchar>(i,j) = color_black;
		}
	}

	morphologyEx( horizon, horizon, MORPH_CLOSE, element);

	if(prevPos != NULL ) delete prevPos;
	prevPos = pos;
	horizon.copyTo( prevHorizon );
}

IMPEXP void* callFactory() {
	return static_cast< void* > (new RealModulSpracovania());
}

int main()
{
	char c;
	String folder;
	String imagePath1, imagePath2, tmp;
	int mode;
	
	RealModulSpracovania modul;
	ModulSpracovania::In in;
	ModulSpracovania::Out out;
	WorldObject object;
	string path = "kostol";
	object.cestyKSuborom.push_back(path);
	object.id = 1;
	Mat image = imread("images\\kostol\\example-0001.jpg");
	
	Mat output;

	in.image.data = image;
	in.recepts.push_back(object);
		
	while((c=getchar()) != 'Q')
	{
		switch(c)
		{
			case 'R':
			case 'r':
				image.copyTo(output);
				modul.init();
				
				out = modul.detekujObjekty(in);
			
				for( int i = 0; i< out.objects.size(); i++)
				{
					RotatedRect rect = out.objects[i].boundary;
					rectangle(output, rect.boundingRect(), Scalar(0, 255, 0), 4);
				}
				resize( output, output, Size(output.cols / 3, output.rows / 3) );
				imshow("FINAL", output);
				waitKey();
				destroyAllWindows();
				break;
			case 'C':
			case 'c':
				break;
			case 'H':
			case 'h':
				output = modul.najdiHorizont(image);
				imshow("FINAL", output);
				waitKey();
				destroyAllWindows();
				break;
		}
	}
	
	return 0;
}
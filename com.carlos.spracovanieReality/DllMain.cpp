#include <windows.h>
#include "../com.carlos.architecture/DllExports.h"
#include "../com.carlos.architecture/modules/class.ModulSpracovania.hpp"
#include "CV.h"
#include <iostream>
#include <ctime>
#include <log4cpp/PropertyConfigurator.hh>

#ifdef _DEBUG
#pragma comment(lib, "../Debug/com.carlos.architecture.lib")
#ifdef CUDA_MODE
#pragma comment(lib, "SiftGPU_d.lib")
#endif
#else
#pragma comment(lib, "../Release/com.carlos.architecture.lib")
#ifdef CUDA_MODE
#pragma comment(lib, "SiftGPU.lib")
#endif
#endif

#ifdef CUDA_MODE
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#endif

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

/** inicializuj premenne */
void RealModulSpracovania::init() {
	const int th = 500;
	detector.hessianThreshold = th;
	setParameters();
	if(!prevHorizon.empty()) prevHorizon.release();
	prevPos = NULL;
#ifdef CUDA_MODE
	char *argv[] = {"-v", "1",  "-p", "640x480", "-cuda", "-di"}; 
	/** parametre pre SiftGPU: 
	*	-v - zobrazit info do konzoly, 
	*	-p - inicialna velkost pre pyramidu,
	*	-cuda - pouzit CUDA implementaciu
	*	-di - dynamicka indexacia pola pri generovani deskriptorov
	*/
	int argc = sizeof(argv) / sizeof(char *);
	siftGpu = new SiftGPU();
	siftGpu->ParseParam(argc, argv);

	matcherGPU.SetMaxSift(4096);
	//Verify current OpenGL Context and do initialization
	if(matcherGPU.CreateContextGL() == 0) { cerr << "Chyba Matcher"; return; }
#endif
}

/** vytvor zoznam zdetegovanych objektov s ich polohami
*	@param candidates mnozina kandidatov na objekt
*   @param objectList mnozina vstupnych potencionalnych objektov na snimke
*	@param objectList mnozina zdetegovanych objektov na snimke
*/
void RealModulSpracovania::getValidObjects(const vector<Candidate> &candidates, const vector<WorldObject> &objectList, vector<DetekovanyObjekt> &validObjects)
{
	int index;

	for( WorldObject object : objectList )
	{
		index = -1;
		for( int i = 0; i < candidates.size(); i++ ) /// najdi najlepsieho validneho kandidate pre dany objekt
		{
			if( candidates[i].valid && object.id == candidates[i].id )
				index = i;
		}

		if( index >= 0 ) /// nasli sme aspon 1 kandidata
		{
			DetekovanyObjekt najdenyObjekt;
			najdenyObjekt.objekt = objectList[ candidates[index].pos ]; /**< detegovany objekt */

			najdenyObjekt.boundary = RotatedRect(
				Point2f( candidates[index].position1.x + ( candidates[index].position2.x - candidates[index].position1.x ) /2, candidates[index].position1.y + ( candidates[index].position2.y - candidates[index].position1.y) / 2 ), 
				Size( candidates[index].position2.x - candidates[index].position1.x ,  candidates[index].position2.y - candidates[index].position1.y ), 
				0
				); /**< pozicia objektu */

			if(log != NULL) {
				/*log->debugStream()*/ cout << "Najdeny objekt ID #" << object.id << " na zaklade kandidata c. #" << index << endl;
				cout << "Pravdepodobnost: " << candidates[index].confidence << endl;
				cout << "Velkost: " << candidates[index].position1 << " -> " << candidates[index].position2 << endl;
			}

			validObjects.push_back(najdenyObjekt);
		}
	}
}

/** detekcia objektov z mnoziny moznych objektov
*	@param in obsahuje mnozinu objektov spolu s ich cestami, ktore sa na vstupnom obrazku mozu nachadzat
*	@return najpravdepodobnejsi objekt na obrazku
*/

ModulSpracovania::Out RealModulSpracovania::detekujObjekty(ModulSpracovania::In in) {
	ModulSpracovania::Out out; /**< najdeny objekt */
	Mat scene; /**< vstupny obrazok */

	vector<KeyPoint> sceneKeyPoints; /**< keypointy streamu */
	Mat sceneDescriptors; /**< matica deskriptorov streamu */

	if( in.image.data.empty() || in.recepts.empty() ) return out; /// chyba nacitania streamu alebo ziadne mozne objekty

	cvtColor( in.image.data, scene, CV_BGR2GRAY );

	if( !in.recepts.empty() ){
		int imported = importCandidates(candidates, in.recepts); /**< pocet kandidatov na objekty */
		if(log != NULL){
			log->debugStream() << "Candidates: " << candidates.size();
		}
	}
	else
		candidates.clear();

	if( candidates.size() == 0 ) return out; /// neuspesny import

#ifdef CUDA_MODE
	runSiftGpu(scene, sceneKeyPoints, sceneDescriptors);
#else
	runSurf(scene, sceneKeyPoints, sceneDescriptors);  /// najdi keypoints a deskriptory
#endif

	for(int i = 0; i < candidates.size(); i++ )
	{
		Mat H; /**< matica perspektivnej transformacie */
		vector<DMatch> matches; /**< zhody */
		robustMatching( candidates[i].descriptors, sceneDescriptors, matches, distanceRatioThreshold ); /// najdi dobre zhody
		//cout << "Matches: " << matches.size() << endl;
		candidates[i].valid = selectInliers( candidates[i].keyPoints, sceneKeyPoints, matches, H ); /// odstran outliers a ziskaj maticu perspektivnej transformacie
		if( candidates[i].valid == false ) continue; /// neuspesna detekcia objektu na vstupnom obrazku
		else
		{
			candidates[i].confidence = computeConfidence( candidates[i].keyPoints.size(), sceneKeyPoints.size(), matches.size() ); /// pomer medzi bodmi a dobrymi zhodami (urcuje spravnost detekcie)
			candidates[i].valid = findObject( candidates[i].image, scene.size(), H, candidates[i].confidence, candidates[i].position1, candidates[i].position2 ); /// najdi poziciu objektu
		}
	}

	//int index = getBestCandidate( candidates ); /**< najlepsi kandidat za objekt */
	//
	//if( index >= 0 ) /// najdenie pozicie najlepsieho kandidata
	//{
	//	DetekovanyObjekt najdenyObjekt;
	//	
	//	najdenyObjekt.objekt = in.recepts[ candidates[index].pos ]; /**< detegovany objekt */
	//	
	//	najdenyObjekt.boundary = RotatedRect(
	//		Point2f( candidates[index].position1.x + ( candidates[index].position2.x - candidates[index].position1.x ) /2, candidates[index].position1.y + ( candidates[index].position2.y - candidates[index].position1.y) / 2 ), 
	//		Size( candidates[index].position2.x - candidates[index].position1.x ,  candidates[index].position2.y - candidates[index].position1.y ), 
	//		0
	//		); /**< pozicia objektu */
	//	
	//	cout << "Najdeny objekt #" << index << endl;
	//	cout << "Pravdepodobnost: " << candidates[index].confidence << endl;
	//	cout << "Hamming. vzdialenost: " << candidates[index].avgDistance << endl;
	//
	//	// Poslem vysledok dalej
	//	out.objects.push_back(najdenyObjekt); /**< vlozit vysledok */
	//}

	getValidObjects( candidates, in.recepts, out.objects ); 

	return out;
}

/** inicializacia kandidata */

Candidate::Candidate():confidence(0.),valid(true)
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
		if( candidates[i].valid && candidates[i].confidence >= 0
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

bool RealModulSpracovania::findObject(const Mat &image, const Size &size, const Mat &H, const double confidence, Point2f &position1, Point2f &position2)
{
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); 
	obj_corners[1] = cvPoint( image.cols, 0 );
	obj_corners[2] = cvPoint( image.cols, image.rows ); 
	obj_corners[3] = cvPoint( 0, image.rows );
	vector<Point2f> scene_corners(4);
	Rect roi;
	const int minWidth = 100;
	const int minHeight = 100;

	if( confidence < minRecognitionConfidence ) return false;

	try{
		perspectiveTransform( obj_corners, scene_corners, H );
		roi = boundingRect( scene_corners );

		position1 = Point2f( roi.x, roi.y );
		if( position1.x < 0 ) position1.x = 0;
		if( position1.y < 0 ) position1.y = 0;

		position2 = Point2f( roi.x + roi.width, roi.y + roi.height );
		if( position2.x > size.width ) position2.x = size.width - 1;
		if( position2.y > size.height ) position2.y = size.height - 1;

		if(abs(position1.x - position2.x) < minWidth || abs(position1.y - position2.y) < minHeight ) return false; /// objekt musi splnat min. velkost

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
#ifndef CUDA_MODE
	char *fileFormat = "../data/objects/%s/image-%04d.yml"; /// cesta ku keypoint-om a deskriptorom
#else
	char *fileFormat = "../data/objects/%s/image-%04dG.yml"; /// cesta ku keypoint-om a deskriptorom
#endif

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
	cout << "Uspesne nacitane" << endl;
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
#ifndef CUDA_MODE
	char *fileFormat = "../data/objects/%s/image-%04d.yml"; /// cesta ku keypoint-om a deskriptorom
#else
	char *fileFormat = "../data/objects/%s/image-%04dG.yml"; /// cesta ku keypoint-om a deskriptorom
#endif

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
	char *fileFormat = "../data/objects/%s/image-%04d.jpg"; /// cesta k objektom
	WorldObject object;
	bool isIn;
	vector<Candidate> tmp_candidates;

	for (int pos = 0; pos < objects.size(); pos++)
	{
		object = objects.at(pos);
		isIn = false;
		if( !candidates.empty() )
		{
			for( Candidate candidate: candidates )
			{
				if( candidate.id == object.id )
				{
					isIn = true;
					tmp_candidates.push_back( candidate );
				}
			}
		}
		while(!isIn)
		{
			Candidate candidate;
			candidate.id = object.id;
			candidate.pos = pos;
			string path = format(fileFormat, object.cestyKSuborom[0].c_str(), count+1);
			candidate.image = imread(path, IMREAD_GRAYSCALE); /// nacitaj obrazky
			if(candidate.image.empty()) break;
#ifdef CUDA_MODE
			///runSiftGpu(candidate.image, candidate.keyPoints, candidate.descriptors)
			if(!readDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors))
			{	
				cout << "Bezim nanovo" << endl;
				runSiftGpu(candidate.image, candidate.keyPoints, candidate.descriptors); /// najdi keypoint-y a deskriptory
				writeDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors);
			}
			count++;
			if(candidate.keyPoints.empty() || candidate.keyPoints.size() < minKeypointsPerObject || candidate.descriptors.empty()) continue;
			tmp_candidates.push_back(candidate);
#else
			// runSurf(candidate.image, candidate.keyPoints, candidate.descriptors); 
			/// pokus sa nacitat keypoint-y a deskriptory zo suboru, inak ich prepocitaj nanovo a zapis do suboru
			if(!readDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors))
			{	
				runSurf(candidate.image, candidate.keyPoints, candidate.descriptors); /// najdi keypoint-y a deskriptory
				writeDescriptors(object.cestyKSuborom[0], count+1, candidate.keyPoints, candidate.descriptors);
			}
			count++;
			if(candidate.keyPoints.empty() || candidate.keyPoints.size() < minKeypointsPerObject || candidate.descriptors.empty()) continue;
			tmp_candidates.push_back(candidate);
#endif
		}
	}

	candidates = tmp_candidates;

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

		}catch(Exception &e)
		{
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

void RealModulSpracovania::setParameters(double distanceRatioThreshold, int minMatchesToFindHomography, bool ransacOutliersRemovalEnabled, int minKeypointsPerObject, double minRecognitionConfidence)
{
	this->distanceRatioThreshold = distanceRatioThreshold, 
	this->minMatchesToFindHomography = minMatchesToFindHomography,
	this->ransacOutliersRemovalEnabled = ransacOutliersRemovalEnabled,
	this->minKeypointsPerObject = minKeypointsPerObject,
#ifdef CUDA_MODE
	this->minRecognitionConfidence = 0.02;// 0.0213;
#else
	this->minRecognitionConfidence = minRecognitionConfidence;
#endif
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
#ifndef CUDA_MODE // TODO - upravit sposob matchovania
	vector<vector<DMatch>> matches12, matches21;
	matches.clear();

	crossKnnMatch(descriptors1, descriptors2, matches12, matches21, 2);

	distanceRatioFilter(matches12, minRatioThreshold);
	distanceRatioFilter(matches21, minRatioThreshold);

	crossCheckFilter(matches12, matches21, matches);
#else
	int (*match_buf)[2] = new int[descriptors1.rows][2];

	matcherGPU.SetDescriptors(0, descriptors1.rows, (float *) descriptors1.data);
	matcherGPU.SetDescriptors(1, descriptors2.rows, (float *) descriptors2.data);

	//Match and read back result to input buffer
	int nmatch = matcherGPU.GetSiftMatch( descriptors1.rows, match_buf, 0.75F, 0.82F, 1); 
	//int nmatch = matcherGPU.GetSiftMatch( descriptors1.rows, match_buf, 0.82F, 0.82F, 1);

	for( int i = 0; i < nmatch; i++ )
	{
		DMatch match;
		match.queryIdx = match_buf[i][0];
		match.trainIdx = match_buf[i][1];

		match.distance = 0;
		for (int j = 0; j < SIFT_SIZE; j++) 
		{ 
			float L2 = *((float *) descriptors1.data + match.queryIdx * SIFT_SIZE + j ) - *((float *) descriptors2.data + match.trainIdx * SIFT_SIZE + j);
			L2 *= L2;
			match.distance += L2;
		}

		match.distance = sqrt( match.distance );

		matches.push_back( match );
	}

	delete[] match_buf; 
#endif
}

/** detegovat keypointy a opisat ich pomocou SURF deskriptorov
*	@param image obrazok
*	@param keypoints keypointy
*	@param descriptors matica deskriptorov
*/

void RealModulSpracovania::runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	detector.detect(image, keypoints);
	extractor.compute(image, keypoints, descriptors);
}

#ifdef CUDA_MODE
/** detegovat keypointy a opisat ich pomocou SIFT deskriptorov (GPU)
*	@param image obrazok
*	@param keypoints keypointy
*	@param descriptors matica deskriptorov
*/

void RealModulSpracovania::runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	if (!siftGpu->RunSIFT(image.cols, image.rows, image.data, GL_LUMINANCE, GL_UNSIGNED_BYTE)) {
		keypoints.resize(0);
		descriptors = Mat();
		cerr << "Chyba SiftGpu" << endl;
		return;
	}

	const int numFeatures = siftGpu->GetFeatureNum();
	SiftGPU::SiftKeypoint *siftKeypoints = new SiftGPU::SiftKeypoint[numFeatures];
	descriptors.create(numFeatures, SIFT_SIZE, CV_32FC1);
	siftGpu->GetFeatureVector(siftKeypoints, (float *) descriptors.data);

	keypoints.resize(0);
	keypoints.reserve(numFeatures);
	
	cout << "KEYPOINTS: " << numFeatures << endl;

	KeyPoint keypoint;
	Point2f *point;

	for (int i = 0; i < numFeatures; ++i) {
		point = (Point2f *) &siftKeypoints[i];

		keypoint.pt = *point;
		keypoint.size = siftKeypoints[i].s;
		keypoint.angle = siftKeypoints[i].o;
		keypoints.push_back(keypoint);
	}

	delete [] siftKeypoints;
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

#ifdef CUDA_MODE
	runSiftGpu(image1, keypoints1, descriptors1);  /// najdi keypoints a deskriptory
	runSiftGpu(image2, keypoints2, descriptors2);  /// najdi keypoints a deskriptory
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

/** najdi horizont na obrazku
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
	std::string initFileName = "../data/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);
	return static_cast< void* > (new RealModulSpracovania());
}

int main()
{
	std::string initFileName = "../data/log4cpp.properties";
	log4cpp::PropertyConfigurator::configure(initFileName);
	
	char c;
	String folder;
	String imagePath1, imagePath2, tmp;
	int mode;

	RealModulSpracovania modul;
	ModulSpracovania::In in;
	ModulSpracovania::Out out;
	WorldObject object;
	string path = "trencin_KM";
	object.cestyKSuborom.push_back(path);
	object.id = 4;
	Mat image; // = imread("images\\kostol\\example-0001.jpg");
	VideoCapture video;
	//video.open("../data/video/blava2.wmv");
	video.open("../data/video/2013-10-20-12-18-22.avi");
	if(!video.isOpened()) return -1;

	Mat output;
	clock_t begin_time;

	//in.image.data = image;
	in.recepts.push_back(object);
	modul.init();

/*
*****POSUN VO VIDEU*******
*/
// KM
	for(int i= 0; i<500; i++) video >> image;

// FTVS UK
//	for(int i= 0; i<3500; i++) video >> image;

// River Park
//	for(int i= 0; i<5250; i++) video >> image;

// Novy most / UFO
//	for(int i= 0; i<10000; i++) video >> image;

// Cafe Propeler
//	for(int i= 0; i<11000; i++) video >> image;

// Reduta
//	for(int i= 0; i<11400; i++) video >> image;

// SNG
//	for(int i= 0; i<13700; i++) video >> image;

// Hotel Danube
//	for(int i= 0; i<14500; i++) video >> image;

// NRSR
//	for(int i= 0; i<17000; i++) video >> image;

// STV
//	for(int i= 0; i<24000; i++) video >> image;
//*************************
cout << "Ready" << endl;

	while((c=getchar()) != 'Q')
	{
		switch(c)
		{
		case 'R':
		case 'r':
			while(true)
			{
				//for(int i= 0; i<15; i++)
					video >> image;
				if(image.empty()) break;

				in.image.data = image;
				image.copyTo(output);

				begin_time = clock();
				out = modul.detekujObjekty(in);
				cout << "Time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;

				for( int i = 0; i< out.objects.size(); i++)
				{
					RotatedRect rect = out.objects[i].boundary;
					rectangle(output, rect.boundingRect(), Scalar(0, 255, 0), 4);
				}
				
				//resize( output, output, Size(output.cols / 3, output.rows / 3) );
				imshow("FINAL", output);
				if( out.objects.size() == 0)
					waitKey(30);
				else
					waitKey();
			}
			destroyAllWindows();
			break;
		case 'C':
		case 'c':
			break;
		case 'H':
		case 'h':
			while(true)
			{
				video >> image;
				if(image.empty()) break;
				begin_time = clock();
				output = modul.najdiHorizont(image);
				cout << "Time: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
				imshow("ORIGIN", image);
				imshow("FINAL", output);
				waitKey(30);
			}
			destroyAllWindows();
			break;
		}
	}

	return 0;
}
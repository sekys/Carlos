#include <windows.h>
#include "../Carlos/architecture/DllExports.h"
#include "../Carlos/architecture/modules/class.ModulSpracovania.hpp"
#include "CV.h"
#include <iostream>


int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	printf("Dll nacitane\n");
	return 1;
}

void RealModulSpracovania::init() {
	const int th = 400;
	detector.hessianThreshold = th;
	setParameters();
#ifdef GPU_MODE
	siftGpu = NULL;
#endif
}

ModulSpracovania::Out RealModulSpracovania::detekujObjekty(In in) {
	Out out;
	Mat scene;
	vector<Candidate> candidates; // kandidati na detekovany objekt
	vector<KeyPoint> sceneKeyPoints; // keypointy streamu
	Mat sceneDescriptors; // matica deskriptorov streamu

	if( in.image.data.empty() || in.recepts.empty() ) return out; // chyba nacitania streamu alebo ziadne mozne objekty
	cvtColor( in.image.data, scene, CV_BGR2GRAY );

#ifdef GPU_MODE
	runSiftGpu(scene, sceneKeyPoints, sceneDescriptors);
#else
	runSurf(scene, sceneKeyPoints, sceneDescriptors);  // najdi keypoints a deskriptory
#endif

	int imported = importCandidates(candidates, in.recepts);
	if( imported == 0 ) return out; // neuspesny import

	for(int i = 0; i < candidates.size(); i++ )
	{
		Mat H;
		vector<DMatch> matches;
		robustMatching( candidates[i].descriptors, sceneDescriptors, matches, distanceRatioThreshold ); // najdi dobre zhody
			
		candidates[i].valid = selectInliers( candidates[i].keyPoints, sceneKeyPoints, matches, H ); // odstran outliers a ziskaj maticu perspektivnej transformacie
		if( candidates[i].valid == false ) continue;
		else
		{
			candidates[i].avgDistance = computeAvgDistance( matches ); // priemerna L2 vzdialenost zhod
			candidates[i].confidence = computeConfidence( candidates[i].keyPoints.size(), sceneKeyPoints.size(), matches.size() ); // pomer medzi bodmi a dobrymi zhodami (urcuje spravnost detekcie)
			candidates[i].valid = findObject( scene, H, candidates[i].confidence, candidates[i].position1, candidates[i].position2 ); // najdi poziciu objektu
		}
	}

	int index = getBestCandidate( candidates ); // vyber najlepsieho kandidata za objekt

	if( index >= 0 )
	{
		DetekovanyObjekt najdenyObjekt;
		najdenyObjekt.objekt = in.recepts[ candidates[index].id ];

		najdenyObjekt.position1.x = candidates[index].position1.x;
		najdenyObjekt.position1.y = candidates[index].position1.y;

		najdenyObjekt.position2.x = candidates[index].position2.x;
		najdenyObjekt.position2.y = candidates[index].position2.y;

		// Poslem vysledok dalej
		out.objects.push_back(najdenyObjekt);
	}
	return out;
}

Candidate::Candidate():confidence(0.),avgDistance(0.),valid(true)
{
}

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

double RealModulSpracovania::computeConfidence(int keypoints1_size, int keypoints2_size, int matches_size)
{
	double confidence = 0;
	confidence = (double) matches_size / ( ( keypoints1_size + keypoints2_size )*0.5 );
	return confidence;
}

bool RealModulSpracovania::findObject(const Mat &image, const Mat &H, double confidence, Point2f &position1, Point2f &position2)
{
	vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0,0); 
	obj_corners[1] = cvPoint( image.cols, 0 );
	obj_corners[2] = cvPoint( image.cols, image.rows ); 
	obj_corners[3] = cvPoint( 0, image.rows );
	vector<Point2f> scene_corners(4);
	Rect roi;
	const int minWidth = 5;
	const int minHeight = 5;

	if( confidence < minRecognitionConfidence ) return false;

	try{
		perspectiveTransform( obj_corners, scene_corners, H );
		roi = boundingRect( scene_corners );
		if(roi.width < minWidth || roi.height < minHeight ) return false;

		position1 = Point2f( roi.x, roi.y );
		position2 = Point2f( roi.x + roi.width, roi.y + roi.height );
	}catch(Exception &e)
	{
		return false;
	}

	return true;
}

int RealModulSpracovania::importCandidates(vector<Candidate> &candidates, const vector<WorldObject> &objects) // nacitaj pre vsetky mozne objekty vsetky obrazky a najdi keypoints a maticu deskriptorov 
{
	int count = 0; // pocet nacitanych kandidatov
	const char *fileFormat = "images\\%s\\image-%04d.jpg"; // zatial bude cesta k suborom takato

	for (int pos = 0; pos < objects.size(); pos++)
	{
		WorldObject object = objects[pos];
		while(true)
		{
			Candidate candidate;
			candidate.id = object.id;
			candidate.image = imread(format(fileFormat, object.cestyKSuborom[0].c_str(), count+1), IMREAD_GRAYSCALE); // nacitaj obrazky
			if(candidate.image.empty()) break;
#ifdef GPU_MODE
			runSiftGpu(candidate.image, candidate.keyPoints, candidate.descriptors);
#else
			runSurf(candidate.image, candidate.keyPoints, candidate.descriptors); // najdi keypoints a deskriptory
#endif
			if(candidate.keyPoints.size() < minKeypointsPerObject || candidate.descriptors.empty()) continue;
			candidates.push_back(candidate);
			count++;
		}
	}

	return count;
}

bool RealModulSpracovania::selectInliers(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H)
{
	vector<Point2f> obj_points;
	vector<Point2f> scene_points;
	vector<uchar> inliers(matches.size(), 0);

	if(sceneKeypoints.size() > minKeypointsPerObject * 2 && matches.size() > minMatchesToFindHomography )
	{
		for( int x = 0; x < matches.size(); x++ )
		{
			//-- Get the keypoints from the good matches
			obj_points.push_back( objectKeypoints[ matches[x].queryIdx ].pt );
			scene_points.push_back( sceneKeypoints[ matches[x].trainIdx ].pt );
		}

		try{
			H = findHomography( obj_points, scene_points, inliers, CV_RANSAC ); // najdi homografiu - maticu perspektivnej transformacie
					
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

void RealModulSpracovania::crossKnnMatch(Mat descriptors1, Mat descriptors2, vector<vector<DMatch>> &matches12, vector<vector<DMatch>> &matches21, int k)
{
	matches12.clear();
	matches21.clear();

	if(descriptors1.empty() || descriptors2.empty()) return;

	matcher.knnMatch(descriptors1,descriptors2,matches12, k);
	matcher.knnMatch(descriptors2,descriptors1,matches21, k);
}

void RealModulSpracovania::distanceRatioFilter(vector<vector<DMatch>> &matches, double th)
{
	for( int i = 0; i < matches.size(); i++ )
	{ 
		if( matches[i].size() == 2 && matches[i][1].distance > 0 && matches[i][0].distance > th*matches[i][1].distance )
			matches[i].clear();
	}
}

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

void RealModulSpracovania::setParameters(double distanceRatioThreshold, int minMatchesToFindHomography, bool ransacOutliersRemovalEnabled, int minKeypointsPerObject, double minRecognitionConfidence, double maxAvgDistance)
{
	this->distanceRatioThreshold = distanceRatioThreshold, 
	this->minMatchesToFindHomography = minMatchesToFindHomography,
	this->ransacOutliersRemovalEnabled = ransacOutliersRemovalEnabled,
	this->minKeypointsPerObject = minKeypointsPerObject,
	this->minRecognitionConfidence = minRecognitionConfidence;
	this->maxAvgDistance = maxAvgDistance;
}

void RealModulSpracovania::distanceMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12;

	matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
	distanceRatioFilter(matches12, 0.7);

	for(int i = 0; i < matches12.size(); i++)
		if(matches12[i].size() > 0) matches.push_back( matches12[i][0] );
}

void RealModulSpracovania::simpleMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches)
{
	matcher.match(descriptors1, descriptors2, matches);
}

void RealModulSpracovania::robustMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12, matches21;
	matches.clear();

	crossKnnMatch(descriptors1, descriptors2, matches12, matches21, 2);

	distanceRatioFilter(matches12, minRatioThreshold);
	distanceRatioFilter(matches21, minRatioThreshold);
	crossCheckFilter(matches12, matches21, matches);
}

void RealModulSpracovania::runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	detector.detect(image, keypoints);
	//if( keypoints.size() > 3000 ) keypoints.resize(3000);
	extractor.compute(image, keypoints, descriptors);
}

#ifdef GPU_MODE
void RealModulSpracovania::runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	char *argv[] = {"-v", "0",  "-p", "1280x960"};
	int argc = sizeof(argv) / sizeof(char *);
	siftGpu = new SiftGPU;
	siftGpu->ParseParam(argc, argv);

	if(siftGpu->CreateContextGL() != SiftGPU::SIFTGPU_FULL_SUPPORTED ) return;
	if (!siftGpu->RunSIFT(image.cols, image.rows, image.data, 0x1909, 0x1401)) {
		keypoints.resize(0);
		descriptors = cv::Mat();
		return;
	}
	
	const int numFeatures = siftGpu->GetFeatureNum();
	SiftGPU::SiftKeypoint *siftKeypoints = new SiftGPU::SiftKeypoint[numFeatures];
	descriptors.create(numFeatures, SIFT_DIMENSIONS, CV_32FC1);
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
}
#endif

Mat RealModulSpracovania::kalibruj(Mat image1, Mat image2) {
	Mat out;

	vector<KeyPoint> keypoints1, keypoints2; // keypointy streamu
	Mat descriptors1, descriptors2; // matica deskriptorov streamu

	if( image1.empty() || image2.empty() ) return out; // chyba nacitania streamu alebo ziadne mozne objekty
	cvtColor( image1, image1, CV_BGR2GRAY );
	cvtColor( image2, image2, CV_BGR2GRAY );

#ifdef GPU_MODE
	runSiftGpu(scene, sceneKeyPoints, sceneDescriptors);
#else
	runSurf(image1, keypoints1, descriptors1);  // najdi keypoints a deskriptory
	runSurf(image2, keypoints2, descriptors2);  // najdi keypoints a deskriptory
#endif

	Mat H;
	vector<DMatch> matches;
	robustMatching( descriptors1, descriptors2, matches, distanceRatioThreshold ); // najdi dobre zhody
			
	if(findMatrix( keypoints1, keypoints2, matches, H ) == false) return out; // odstran outliers a ziskaj maticu perspektivnej transformacie
	
	out= H;
	return out;
}

bool RealModulSpracovania::findMatrix(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H)
{
	vector<Point2f> obj_points;
	vector<Point2f> scene_points;
	vector<uchar> inliers(matches.size(), 0);

	if(sceneKeypoints.size() > minKeypointsPerObject * 2 && matches.size() > minMatchesToFindHomography )
	{
		for( int x = 0; x < matches.size(); x++ )
		{
			//-- Get the keypoints from the good matches
			obj_points.push_back( objectKeypoints[ matches[x].queryIdx ].pt );
			scene_points.push_back( sceneKeypoints[ matches[x].trainIdx ].pt );
		}

		try{
			H = findHomography( obj_points, scene_points, inliers, CV_RANSAC ); // najdi homografiu - maticu perspektivnej transformacie
		}catch(Exception &e){
			return false;
		}
	}
	else return false;

	if( matches.size() < minMatchesToFindHomography ) return false;

	return true;
}

Mat RealModulSpracovania::najdiHorizont(Mat image)
{
	Mat horizon;
	cvtColor( image, image, CV_BGR2GRAY );
	cannyHorizonDetection(image, horizon);
	return horizon;
}

void RealModulSpracovania::cannyHorizonDetection(const Mat &image, Mat &horizon)
{
	const int ratio = 3;
	const int kernel_size = 3;
	const int lowThreshold = 130;
	Mat detected_edges;

	horizon.create( image.size(), CV_8UC1 );

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	/// Reduce noise with a kernel 3x3
	blur( image, detected_edges, Size(3,3) );

	/// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
	threshold( detected_edges, detected_edges, lowThreshold, 255, CV_THRESH_BINARY );
	findContours( detected_edges, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	horizon = Scalar::all(0);

	for( int i = 0; i< contours.size(); i++ )
	{
		drawContours( horizon, contours, i, Scalar::all(255), 2, 8, hierarchy);
	}

	for( int j= 0; j < horizon.cols; j++ )
	{
		int color = 255;
		for( int i = 0; i < horizon.rows; i++ )
		{
			if(horizon.at<uchar>(i,j) != 0 && color == 255) color = 0;
			horizon.at<uchar>(i,j) = color;
		}
	}

	morphologyEx( horizon, horizon, MORPH_DILATE, Mat(), Point(), 1 );
	morphologyEx( horizon, horizon, MORPH_ERODE, Mat(), Point(), 1 );
}

IMPEXP void* callFactory() {
	return static_cast< void* > (new RealModulSpracovania());
}
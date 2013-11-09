#include "ImageRegistration.h"

using namespace Architecture;
using namespace cv;

RegistraciaObrazu::RegistraciaObrazu(){
	const int th = 400;
	detector.hessianThreshold = th;
	setParameters();
#ifdef GPU_MODE
	siftGpu = NULL;
#endif
}

ModulKalibracie::Out RegistraciaObrazu::kalibruj(In in) {
	Out out;
	Mat image1, image2;

	vector<KeyPoint> keypoints1, keypoints2; // keypointy streamu
	Mat descriptors1, descriptors2; // matica deskriptorov streamu

	if( in.image1.data.empty() || in.image2.data.empty() ) return out; // chyba nacitania streamu alebo ziadne mozne objekty
	cvtColor( in.image1.data, image1, CV_BGR2GRAY );
	cvtColor( in.image2.data, image2, CV_BGR2GRAY );

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
	
	out.H = H;
	return out;
}

bool RegistraciaObrazu::findMatrix(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H)
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

void RegistraciaObrazu::crossCheckFilter(vector<vector<DMatch>> matches12, vector<vector<DMatch>> matches21, vector<DMatch> &matches)
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

void RegistraciaObrazu::crossKnnMatch(Mat descriptors1, Mat descriptors2, vector<vector<DMatch>> &matches12, vector<vector<DMatch>> &matches21, int k)
{
	matches12.clear();
	matches21.clear();

	if(descriptors1.empty() || descriptors2.empty()) return;

	matcher.knnMatch(descriptors1,descriptors2,matches12, k);
	matcher.knnMatch(descriptors2,descriptors1,matches21, k);
}

void RegistraciaObrazu::distanceRatioFilter(vector<vector<DMatch>> &matches, double th)
{
	for( int i = 0; i < matches.size(); i++ )
	{ 
		if( matches[i].size() == 2 && matches[i][1].distance > 0 && matches[i][0].distance > th*matches[i][1].distance )
			matches[i].clear();
	}
}

void RegistraciaObrazu::setParameters(double distanceRatioThreshold, int minMatchesToFindHomography, bool ransacOutliersRemovalEnabled, int minKeypointsPerObject, double minRecognitionConfidence, double maxAvgDistance)
{
	this->distanceRatioThreshold = distanceRatioThreshold, 
	this->minMatchesToFindHomography = minMatchesToFindHomography,
	this->ransacOutliersRemovalEnabled = ransacOutliersRemovalEnabled,
	this->minKeypointsPerObject = minKeypointsPerObject,
	this->minRecognitionConfidence = minRecognitionConfidence;
	this->maxAvgDistance = maxAvgDistance;
}

void RegistraciaObrazu::distanceMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12;

	matcher.knnMatch(descriptors1, descriptors2, matches12, 2);
	distanceRatioFilter(matches12, 0.7);

	for(int i = 0; i < matches12.size(); i++)
		if(matches12[i].size() > 0) matches.push_back( matches12[i][0] );
}

void RegistraciaObrazu::simpleMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches)
{
	matcher.match(descriptors1, descriptors2, matches);
}

void RegistraciaObrazu::robustMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold)
{
	vector<vector<DMatch>> matches12, matches21;
	matches.clear();

	crossKnnMatch(descriptors1, descriptors2, matches12, matches21, 2);

	distanceRatioFilter(matches12, minRatioThreshold);
	distanceRatioFilter(matches21, minRatioThreshold);
	crossCheckFilter(matches12, matches21, matches);
}

void RegistraciaObrazu::runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
{
	detector.detect(image, keypoints);
	//if( keypoints.size() > 3000 ) keypoints.resize(3000);
	extractor.compute(image, keypoints, descriptors);
}

#ifdef GPU_MODE
void RegistraciaObrazu::runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors)
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
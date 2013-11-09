#define __dll__
#include "DllExports.h"
#include <windows.h>
#include "architecture\modules\class.ModulSpracovania.hpp"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2\nonfree\gpu.hpp>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\gpu\gpu.hpp>
#define GPU_MODE
#undef GPU_MODE

#ifdef GPU_MODE
#include <SiftGPU\SiftGPU.h>
#endif

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*) {
	return 1;
}

using namespace Architecture;
using namespace cv;

class Candidate {
public:
	int id;
	vector<KeyPoint> keyPoints;
	Mat descriptors;
	Mat image;
	bool valid;
	double confidence;
	double avgDistance;
	Point2f position1, position2;

	Candidate();
};

class DetekciaObjektov : public ModulSpracovania {
	virtual Out detekujObjekty(In in);

public:
	DetekciaObjektov();
private:
	BruteForceMatcher<L2<float>> matcher;
	SurfFeatureDetector detector;
	SurfDescriptorExtractor extractor;

	double distanceRatioThreshold;
	int minMatchesToFindHomography;
	bool ransacOutliersRemovalEnabled;
	int minKeypointsPerObject;
	double minRecognitionConfidence;
	double maxAvgDistance;
	
private:
	void setParameters(double distanceRatioThreshold = 0.85, int minMatchesToFindHomography = 5, bool ransacOutliersRemovalEnabled = true, 
		int minKeypointsPerObject = 10, double minRecognitionConfidence = 0.0014 /*0.01*/, double maxAvgDistance = 0.21 /*0.35*/ );
	int importCandidates(vector<Candidate> &candidates, const vector<WorldObject> &objects);
	double computeAvgDistance(const vector<DMatch> &matches);
	double computeConfidence(int keypoints1_size, int keypoints2_size, int matches_size);
	void crossCheckFilter(vector<vector<DMatch>> matches12, vector<vector<DMatch>> matches21, vector<DMatch> &matches);
	void crossKnnMatch(Mat descriptors1, Mat descriptors2, vector<vector<DMatch>> &matches12, vector<vector<DMatch>> &matches21, int k);
	void distanceRatioFilter(vector<vector<DMatch>> &matches, double th);
	void extractInlierMatches(vector<DMatch> &matches, vector<uchar> inliers);
	void robustMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold);
	bool selectInliers(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H);
	bool findObject(const Mat &image, const Mat &H, double confidence, Point2f &position1, Point2f &position2);
	void distanceMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches, double minRatioThreshold);
	void simpleMatching(const Mat &descriptors1, const Mat &descriptors2, vector<DMatch> &matches);
	int getBestCandidate(const vector<Candidate> &candidates);
	void runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors);
#ifdef GPU_MODE
	void runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors);
#endif
};

int maximum(int a, int b)
{
	if(a > b) return a;
	return b;
}

IMPEXP void* callFactory() {
	return static_cast< void* > (new DetekciaObjektov());
}
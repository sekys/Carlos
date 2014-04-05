/** @file CV.h
* Trieda @b Candidate je potencialny kandidat na na objekt
* Trieda @b RealModulSpracovania sa stara o celu funkcionalitu spracovanie obrazu
*/

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\features2d\features2d.hpp>
#include <opencv2\nonfree\nonfree.hpp>
#include <opencv2/nonfree/gpu.hpp>
#include <opencv2\legacy\legacy.hpp>
#include <windows.h>
//#define GPU_MODE
#undef GPU_MODE

#ifdef GPU_MODE
#define SIFT_SIZE 128
#include <SiftGPU\SiftGPU.h>
#include <GL\glew.h>
#include <GL\GL.h>
#endif

using namespace Architecture;
using namespace cv;

/** Funkcia vrati maximum z 2 cisel
* @param a prve cislo
* @param b druhe cislo
* 
*/
int maximum(int a, int b)
{
	if(a > b) return a;
	return b;
}

/** Kandidat na objekt */

class Candidate {
public:
	int id; /**< id objektu */
	int pos; /**< pozicia kandidata (od nuly) */
	vector<KeyPoint> keyPoints; /**< keypointy detegovane na obrazku */
	Mat descriptors; /**< matica deskriptorov detegovanych keypointov */
	Mat image; /**< obrazok objektu */
	bool valid; /**< uspesna detekcia? */
	double confidence; /**< pravdepodobnost ze sa jedna o hladany objekt */
	double avgDistance; /**< priemerna L2 vsetkych matchov */
	Point2f position1; /**< lavy horny roh najdeneho objektu */ 
	Point2f position2; /**< pravy dolny roh najdeneho objektu */ 

	Candidate();
};

class RealModulSpracovania : public ModulSpracovania {

public:
	virtual ModulSpracovania::Out detekujObjekty(In in);
	virtual void init();
	virtual Mat kalibruj(Mat image1, Mat image2);
	virtual Mat najdiHorizont(Mat image);

private:
#ifdef GPU_MODE
	SiftGPU *siftGpu;
	SiftMatchGPU matcherGPU;
#endif
	BruteForceMatcher<L2<float>> matcher; /**< matcher deskriptorov */
	SurfFeatureDetector detector; /**< detektor keypointov */
	SurfDescriptorExtractor extractor; /**< extraktor deskriptorov z keypointov */
	vector<Candidate> candidates; /**< kandidati na detekovany objekt */
	Mat prevHorizon;
	int *prevPos;

	/** Thresholdy: */

	double distanceRatioThreshold; /**< min. vzdialenost medzi prvym a druhym najpravdepodobnejsim matchom */
	int minMatchesToFindHomography; /**< min. pocet matchov */
	bool ransacOutliersRemovalEnabled; /**< odstranenie outliers */
	int minKeypointsPerObject; /**< min. pocet keypointov na objekt */
	double minRecognitionConfidence; /**< min. pravdepodobnost na najdeny objekt */
	double maxAvgDistance; /**< max. L2 vsetkych matchov */
	
private:
	void setParameters(double distanceRatioThreshold = 0.85, int minMatchesToFindHomography = 5, bool ransacOutliersRemovalEnabled = true, 
		int minKeypointsPerObject = 10, double minRecognitionConfidence = 0.004, double maxAvgDistance = 0.25 /*0.35*/ );
	int importCandidates(vector<Candidate> &candidates, vector<WorldObject> &objects);
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
	void getValidObjects(const vector<Candidate> &candidates, const vector<WorldObject> &objectList, vector<DetekovanyObjekt> &validObjects);
	void runSurf(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors);
	bool findMatrix(const vector<KeyPoint> &objectKeypoints, const vector<KeyPoint> &sceneKeypoints, vector<DMatch> &matches, Mat &H);
	void cannyHorizonDetection(const Mat &image, Mat &horizon);
	bool readDescriptors(const string path, const int id, vector<KeyPoint> &keyPoints, Mat &descriptors);
	bool writeDescriptors(const string path, const int id, vector<KeyPoint> keyPoints, Mat descriptors);
#ifdef GPU_MODE
	void runSiftGpu(const Mat &image, vector<KeyPoint> &keypoints, Mat &descriptors);
#endif
};
#include "ppgDetector.h"
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace cv;

string testFolder = "../data/test/";

void readImg(string filename, Mat& img) {
	img = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
}

bool vectorAnd(std::vector<bool> vec, std::vector<int> rank) {
	bool flag = true;
	for (int i = 0; i < rank.size(); ++i) {
		flag = flag && vec[rank[i]];
	}
	return flag;
}

bool testNotOneFace(Detector detector, string filename1, string filename2) {
	detector.initialize();
	Mat frame1, frame2;
	readImg(testFolder + filename1, frame1);
	readImg(testFolder + filename2, frame2);
	Scalar value(-1, -1, -1);
	std::vector<bool> coverage;
	try {
		value = detector.detect(frame1);
		coverage = detector.getCoverage();
	}
	catch (exception) {
		return false;
	}
	std::vector<int> rank = { 0, 1, 10, 11 };
	if (vectorAnd(coverage, rank) && value == Scalar(0, 0, 0))
		return true;
	return false;
}

bool testFirstFrame(Detector detector, string filename1, string filename2) {
	detector.initialize();
	Mat frame1, frame2;
	readImg(testFolder + filename1, frame1);
	readImg(testFolder + filename2, frame2);
	Scalar value(-1, -1, -1);
	std::vector<bool> coverage;
	try {
		value = detector.detect(frame1);
		coverage = detector.getCoverage();
	}
	catch (exception) {
		return false;
	}
	std::vector<int> rank = { 0, 1, 2, 5, 6, 9, 11 };
	if (vectorAnd(coverage, rank) && value != Scalar(0, 0, 0))
		return true;
	return false;
}

bool testNoSmooth(Detector detector, string filename1, string filename2) {
	detector.initialize();
	Mat frame1, frame2;
	readImg(testFolder + filename1, frame1);
	readImg(testFolder + filename2, frame2);
	Scalar value(-1, -1, -1);
	std::vector<bool> coverage;
	try {
		value = detector.detect(frame1);
		coverage = detector.getCoverage();
		value = detector.detect(frame2);
		coverage = detector.getCoverage();
	}
	catch (exception) {
		return false;
	}
	std::vector<int> rank = { 0, 1, 2, 3, 5, 6, 7, 9, 11 };
	if (vectorAnd(coverage, rank) && value != Scalar(0, 0, 0))
		return true;
	return false;
}

bool testSmoothAll(Detector detector, string filename1, string filename2) {
	detector.initialize();
	Mat frame1, frame2;
	readImg(testFolder + filename1, frame1);
	readImg(testFolder + filename2, frame2);
	Scalar value(-1, -1, -1);
	std::vector<bool> coverage;
	try {
		value = detector.detect(frame1);
		coverage = detector.getCoverage();
		value = detector.detect(frame2);
		coverage = detector.getCoverage();
	}
	catch (exception) {
		return false;
	}
	std::vector<int> rank = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
	if (vectorAnd(coverage, rank) && value != Scalar(0, 0, 0))
		return true;
	return false;
}

int main() {
	typedef bool(*testFunc)(Detector, string, string);
	std::vector<testFunc> tests = { testNotOneFace, testFirstFrame, testNoSmooth, testSmoothAll};
	std::vector<string> files = { "Detector_detect_0.bmp", "", "Detector_detect_1.bmp", "", \
		"Detector_detect_1.bmp", "Detector_detect_1_far.bmp", \
		"Detector_detect_1.bmp", "Detector_detect_1_close.bmp" };

	int passNum = 0;
	Detector detector;
	for (int i = 0; i < tests.size(); ++i) {
		cout << "Test Case " << to_string(i + 1) << endl;
		if (tests[i](detector, files[2*i], files[2*i+1])) {
			cout << "  Passed." << endl;
			passNum++;
		}
		else {
			cout << "  Failed!" << endl;
		}
	}
	cout << endl << passNum << "/" << tests.size() << " passed" << endl << endl;

	system("pause");
	return 0;
}
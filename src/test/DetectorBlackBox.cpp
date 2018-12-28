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

bool testEmpty(Detector detector, string filename) {
	Mat frame;
	detector.initialize();
	Scalar value(-1, -1, -1);
	try {
		value = detector.detect(frame);
	}
	catch (exception) {
		return false;
	}
	if (value == Scalar(0, 0, 0))
		return true;
	return false;
}

bool testZero(Detector detector, string filename) {
	Mat frame;
	readImg(testFolder + filename, frame);
	detector.initialize();
	Scalar value(-1, -1, -1);
	try {
		value = detector.detect(frame);
	}
	catch (exception) {
		return false;
	}
	if (value == Scalar(0, 0, 0))
		return true;
	return false;
}

bool testHalf(Detector detector, string filename) {
	Mat frame;
	readImg(testFolder + filename, frame);
	detector.initialize();
	Scalar value(-1, -1, -1);
	try {
		value = detector.detect(frame);
	}
	catch (exception) {
		return false;
	}
	if (value != Scalar(0, 0, 0))
		return true;
	return false;
}

bool testOne(Detector detector, string filename) {
	Mat frame;
	readImg(testFolder + filename, frame);
	detector.initialize();
	Scalar value(-1, -1, -1);
	try {
		value = detector.detect(frame);
	}
	catch (exception) {
		return false;
	}
	if (value != Scalar(0, 0, 0))
		return true;
	return false;
}

bool testTwo(Detector detector, string filename) {
	Mat frame;
	readImg(testFolder + filename, frame);
	detector.initialize();
	Scalar value(-1, -1, -1);
	try {
		value = detector.detect(frame);
	}
	catch (exception) {
		return false;
	}
	if (value == Scalar(0, 0, 0))
		return true;
	return false;
}

int main(){
	typedef bool(*testFunc)(Detector, string);
	std::vector<testFunc> tests = { testEmpty, testZero, testHalf, testOne, testTwo };
	std::vector<string> files = { "", "Detector_detect_0.bmp", "Detector_detect_half.bmp", \
		"Detector_detect_1.bmp", "Detector_detect_2.bmp" };
	int passNum = 0;
	Detector detector;
	for (int i = 0; i < tests.size(); ++i) {
		cout << "Test Case " << to_string(i + 1) << endl;
		if (tests[i](detector, files[i])) {
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
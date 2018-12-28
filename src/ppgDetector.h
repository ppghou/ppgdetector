#ifndef __DETECTOR__
#define __DETECTOR__

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <chrono>
#include <algorithm>
#include <cmath>
<<<<<<< HEAD
#include "utils.h"

#define min(a,b) ((a)>(b) ? b:a)
#define max(a,b) ((a)<(b) ? b:a)
=======
#include "ppgFilter.h"
#include "utils.hpp"
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a

#define min(a,b) ((a)>(b) ? b:a)
#define max(a,b) ((a)<(b) ? b:a)

using namespace dlib;
using namespace cv;
using namespace std;
using namespace chrono;

class Detector{
private:
<<<<<<< HEAD
    int detectWidth;
    double roiRatio;
    double rectSmoothRatio;
    double rectDistThres;
    double markSmoothRatio;
    double markDistThres1;
    double markDistThres2;

    // Detection parameters
    frontal_face_detector detector;
    shape_predictor predictor;
    std::vector<std::vector<int>> idx;

    // Detection results of last frame
    dlib::rectangle rect;
    std::vector<double> xMarks, yMarks;
    Mat detectedImg;

public:
    Detector();

    void initialize();

    Scalar detect(Mat& image);

    double distForRects(dlib::rectangle rect1, dlib::rectangle rect2);

    double distForMarks(dlib::rectangle rect1, dlib::rectangle rect2);

    void smoothRects(dlib::rectangle &rect1, dlib::rectangle &rect2, double smoothRatio);
=======
	int detectWidth;
	double roiRatio;
	double rectSmoothRatio;
	double rectDistThres;
	double markSmoothRatio;
	double markDistThres1;
	double markDistThres2;

	// Detection parameters
	frontal_face_detector detector;
	shape_predictor predictor;
	std::vector<std::vector<int>> idx;

	// Detection results of last frame
	dlib::rectangle rect;
	std::vector<double> xMarks, yMarks;
	Mat detectedImg;

public:
	Detector();
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a

    void smoothMarks(std::vector<double> &xMarks1, std::vector<double> &yMarks1,
            std::vector<double> &xMarks2, std::vector<double> &yMarks2, double smoothRatio);

    void myResize(Mat& image, Mat& resized, Size& new_size, int width);
    
    Rect npToRect(std::vector<int> &rank, std::vector<double> xMarks,
            std::vector<double> yMarks, double ratio);

<<<<<<< HEAD
    dlib::rectangle coordTrans(Size imshape, Size oriSize, dlib::rectangle rect);
=======
	double distForRects(dlib::rectangle rect1, dlib::rectangle rect2);
	
	void smoothRects(dlib::rectangle &rect1, dlib::rectangle &rect2, double smoothRatio);

	double distForMarks(dlib::rectangle rect1, dlib::rectangle rect2);

	void smoothMarks(std::vector<double> &xMarks1, std::vector<double> &yMarks1, \
		std::vector<double> &xMarks2, std::vector<double> &yMarks2, double smoothRatio);

	Rect Detector::npToRect(std::vector<int> &rank, std::vector<double> xMarks, \
		std::vector<double> yMarks, double ratio);

	void myResize(Mat& image, Mat& resized, Size& new_size, int width);

	dlib::rectangle coordTrans(Size imshape, Size oriSize, dlib::rectangle rect);

	Mat getImg();
>>>>>>> 8d65bf9dd02952114a8c4a1e39aed0ab305d258a
};
#endif

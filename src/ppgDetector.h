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
#include "Filter.h"
#include "utils.hpp"

using namespace dlib;
using namespace cv;
using namespace std;
using namespace chrono;

template<int N, int N_tot>
Rect np_to_rect(int(&rank)[N], double(&marks_x)[N_tot], \
	double(&marks_y)[N_tot], double ratio);

void myResize(Mat& image, Mat& resized, Size& new_size, int width);

Mat clip(Mat& image, Size& size, dlib::rectangle rect);

class Detector{
private:
	double roiRatio;
	double smoothRatio;

	// Detection parameters
	frontal_face_detector detector;
	shape_predictor predictor;

	// Detection results of last frame
	dlib::rectangle rect;
	double marks_x[68];
	double marks_y[68];

	// Pointer to a Filter instance
	Filter* filter;

public:
	Detector(Filter* f);

	void initialize();

	Scalar detect(Mat& image);

	double dist(dlib::rectangle rect1, dlib::rectangle rect2);
};
#endif
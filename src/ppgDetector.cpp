#include "ppgDetector.h"

Detector::Detector() {
	detector = get_frontal_face_detector();
	deserialize("../data/shape_predictor_68_face_landmarks.dat") >> predictor;
	initialize();
}

void Detector::initialize() {
	detectWidth = 500;
	roiRatio = 5;
	rectSmoothRatio = 0.98;
	rectDistThres = 4;
	markSmoothRatio = 0.95;
	markDistThres1 = 0.02;
	markDistThres2 = 0.025;

	clearVector(idx);
	std::vector<int> tmpL = { 1, 3, 31 };
	std::vector<int> tmpR = { 13, 15, 35 };
	idx.push_back(tmpL);
	idx.push_back(tmpR);

	clearResults();

	branchNum = 12;
	clearVector(coverage);
	for (int i = 0; i < branchNum; ++i)
		coverage.push_back(false);
}

void Detector::clearResults() {
	clearVector(xMarks);
	clearVector(yMarks);
	rect = dlib::rectangle(0, 0, 0, 0);
	detectedImg = Mat();
}

Scalar Detector::detect(Mat& image) {
	/*Detect the face region and returns the ROI value
	Face detection is the slowest part.
	Args:
	image: a cv::Mat, the image
	Return :
	val: a cv::Scalar, the ROI value of the current frame
	*/

	// Resize the image to limit the calculation
	if (image.empty())
		return Scalar(0, 0, 0);

	Size detectSize = Size();
	Mat resized;
	myResize(image, resized, detectSize, detectWidth);

	// Perform face detection on a grayscale image
	Mat gray;
	cvtColor(resized, gray, CV_RGB2GRAY);

	//Convert the image from the cv format (Mat) to the dlib format (matrix<>)
	matrix<uchar> gray_dlib;
	assign_image(gray_dlib, cv_image<uchar>(gray));

	// No need for upsample, because its effect is the same as resize
	std::vector<dlib::rectangle> rects = detector(gray_dlib);
	coverage[0] = true;

	int num = rects.size();
	coverage[1] = true;
	if (num == 0) {
		coverage[10] = true;
		cout << "No face in the frame!" << endl;
		clearResults();
		coverage[11] = true;
		return Scalar();
	}
	if (num >= 2) {
		coverage[10] = true;
		cout << "More than one face!" << endl;
		clearResults();
		coverage[11] = true;
		return Scalar();
	}
	dlib::rectangle rect = rects[0];

	// If not the first image, perform face region smoothing
	coverage[2] = true;
	if ((this->rect.right() - this->rect.left()) != 0) {
		coverage[3] = true;
		double dist = distForRects(this->rect, rect);
		if (dist < rectDistThres) {
			coverage[4] = true;
			double smoothRatio = rectSmoothRatio * \
				sqrt(1 - dist / rectDistThres);
			smoothRects(this->rect, rect, smoothRatio);
		}
	}

	// Perform landmark prediction on the face region
	dlib::rectangle face = coordTrans(image.size(), detectSize, rect);
	matrix<rgb_pixel> dlibImage;
	assign_image(dlibImage, cv_image<bgr_pixel>(image));

	auto shape = predictor(dlibImage, face);
	coverage[5] = true;

	std::vector<double> xMarks, yMarks;
	for (int i = 0; i < shape.num_parts(); i++) {
		point p = shape.part(i);
		xMarks.push_back(p.x());
		yMarks.push_back(p.y());
	}

	// If not the first image, perform landmark smoothing
	coverage[6] = true;
	if ((this->rect.right() - this->rect.left()) != 0) {
		coverage[7] = true;
		double dist = distForMarks(this->rect, rect);
		if (dist < markDistThres2) {
			coverage[8] = true;
			double tmp = dist - markDistThres1;
			double smoothRatio = 0;
			if (tmp < 0)
				smoothRatio = markSmoothRatio;
			else
				smoothRatio = exp(-1e3 * tmp);
			smoothMarks(this->xMarks, this->yMarks, xMarks, yMarks, smoothRatio);
		}
	}

	// Calculate the ROI values
	std::vector<Rect> roi;
	Scalar val;
	for (int i = 0; i < idx.size(); ++i) {
		roi.push_back(npToRect(idx[i], xMarks, yMarks, roiRatio));
		val = val + mean(image(roi[i]));
	}
	val = val / double(idx.size());
	coverage[9] = true;

	// Save detection results
	this->rect = rect;
	this->xMarks.swap(xMarks);
	this->yMarks.swap(yMarks);

	// Show detection results 
	for (int i = 0; i < roi.size(); ++i) {
		cv::rectangle(image, roi[i], Scalar(0, 0, 255));
	}
	for (int i = 0; i < xMarks.size(); i++) {
		cv::putText(image, to_string(i), Point(xMarks[i], yMarks[i]), \
			cv::FONT_HERSHEY_SIMPLEX, 0.3, Scalar(255, 0, 0));
	}
// 	int left = max(face.left(), 0);
// 	int top = max(face.top(), 0);
// 	int width = min(face.right(), image.size().width) - left;
// 	int height = min(face.bottom(), image.size().height) - top;
// 	Rect CVface(left, top, width, height);
// 	myResize(image(CVface), detectedImg, detectSize, detectWidth);
	coverage[11] = true;
	return val;
}

double Detector::distForRects(dlib::rectangle rect1, dlib::rectangle rect2) {
	/*
	Calculate the distance between two rectangles for rectangle smoothing
	Arg :
	rect1, rect2 : dlib.rectangle
	Return :
	distance between rectangles
	*/
	double distx = (rect1.left() - rect2.left()) + (rect1.left() - rect2.left());
	double disty = (rect1.top() - rect2.top()) + (rect1.bottom() - rect2.bottom());
	return sqrt(pow(distx / (rect1.right() - rect1.left()), 2) +
		pow(disty / (rect1.bottom() - rect1.top()), 2));
}

void Detector::smoothRects(dlib::rectangle &rect1, dlib::rectangle &rect2, double smoothRatio){
	double left = int(round(smoothRatio * rect1.left() +
		(1 - smoothRatio) * rect2.left()));
	double right = int(round(smoothRatio * rect1.right() +
		(1 - smoothRatio) * rect2.right()));
	double top = int(round(smoothRatio * rect1.top() +
		(1 - smoothRatio) * rect2.top()));
	double bottom = int(round(smoothRatio * rect1.bottom() +
		(1 - smoothRatio) * rect2.bottom()));
	rect2 = dlib::rectangle(left, top, right, bottom);
}

double Detector::distForMarks(dlib::rectangle rect1, dlib::rectangle rect2) {
	/*
	Calculate the distance between two rectangles for landmark smoothing
	Arg :
	rect1, rect2 : dlib.rectangle
	Return :
	distance between rectangles
	*/
	double distx = abs(rect1.left() - rect2.left()) \
		+ abs(rect1.right() - rect2.right());
	double disty = abs(rect1.top() - rect2.top()) \
		+ abs(rect1.bottom() - rect2.bottom());
	return abs(distx / (rect1.right() - rect1.left())) \
		+ abs(disty / (rect1.bottom() - rect1.top()));
}

void Detector::smoothMarks(std::vector<double> &xMarks1, std::vector<double> &yMarks1, \
	std::vector<double> &xMarks2, std::vector<double> &yMarks2, double smoothRatio) {
	for (int i = 0; i < xMarks1.size(); i++) {
		xMarks2[i] = smoothRatio * xMarks1[i] + \
			(1 - smoothRatio) * xMarks2[i];
		yMarks2[i] = smoothRatio * yMarks1[i] + \
			(1 - smoothRatio) * yMarks2[i];
	}
}

Rect Detector::npToRect(std::vector<int> &rank, std::vector<double> xMarks, \
	std::vector<double> yMarks, double ratio = 5) {
	/*
	Calculate the roi with surrounding points and ratio
	Args:
	rank: the rank of selected points
	marks_x, marks_y: the coordinates of landmarks
	ratio: the ratio of the length of the bounding box in each direction
	to the distance between ROI and the bounding box
	Returns:
	a cv::Rect, the roi
	*/
	std::vector<double> x, y;
	for (int i = 0; i < rank.size(); i++) {
		x.push_back(xMarks[rank[i]]);
		y.push_back(yMarks[rank[i]]);
	}
	double xMax = *max_element(x.begin(), x.end());
	double yMax = *max_element(y.begin(), y.end());
	double xMin = *min_element(x.begin(), x.end());
	double yMin = *min_element(y.begin(), y.end());
	double p = ratio - 1;
	double q = ratio;
	int left = int(xMin * p / q + xMax / q);
	int right = int(xMax * p / q + xMin / q);
	int top = int(yMin * p / q + yMax / q);
	int bottom = int(yMax * p / q + yMin / q);
	return Rect(left, top, right - left, bottom - top);
}

void Detector::myResize(Mat& image, Mat& resized, Size& newSize, int width = 600) {
	/*
	Resize the image with width
	Args:
		image: a cv::Mat, the image
		resized: a cv::Mat, this is where the output will be stored
		width : the width of the resized image
		size : [width, hight], size of the resized image
	*/
	Size imgSize = image.size();
	double r = width * 1.0 / imgSize.width;
	newSize.width = width;
	newSize.height = int(imgSize.height * r);
	cv::resize(image, resized, newSize);
}

dlib::rectangle Detector::coordTrans(Size imShape, Size oriSize, dlib::rectangle rect) {
	/*
	Transform the coordinates into the original image
	Args:
		imShape: shape of the detected image
		oriSize: size of the original image
		rect: an instance of dlib.rectangle, the face region
	Returns:
		the rect in the original image
	*/
	int left = int(round(double(rect.left()) / oriSize.width * imShape.width));
	int right = int(round(double(rect.right()) / oriSize.width * imShape.width));
	int top = int(round(double(rect.top()) / oriSize.height * imShape.height));
	int bottom = int(round(double(rect.bottom()) / oriSize.height * imShape.height));
	return dlib::rectangle(left, top, right, bottom);
}

std::vector<bool> Detector::getCoverage() {
	std::vector<bool> tmp = coverage;
	clearVector(coverage);
	for (int i = 0; i < branchNum; ++i)
		coverage.push_back(false);
	return tmp;
}

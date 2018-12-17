#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/opencv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <algorithm>

using namespace dlib;
using namespace cv;
using namespace std;
using namespace chrono;

template<int N, int N_tot>
Rect np_to_rect(int (&rank)[N],double (&marks_x)[N_tot], \
    double (&marks_y)[N_tot], double ratio = 5) {
    double x[N] = { 0 };
    double y[N] = { 0 };
    for (int i = 0; i < N; i++) {
        x[i] = marks_x[rank[i]];
        y[i] = marks_y[rank[i]];
    }
    double max_x = *max_element(x, x + N);
    double max_y = *max_element(y, y + N);
    double min_x = *min_element(x, x + N);
    double min_y = *min_element(y, y + N);
    double p = ratio - 1;
    double q = ratio;
    int left = int(min_x * p / q + max_x / q);
    int right = int(max_x * p / q + min_x / q);
    int top = int(min_y * p / q + max_y / q);
    int bottom = int(max_y * p / q + min_y / q);
    return Rect(left, top, right - left, bottom - top);
}

void myResize(Mat& image, Mat& resized, Size& new_size, int width = 1200) {
    /*
    Resize the image with width
    Args:
        image: a Mat, the image
        resized: a Mat, this is where the output will be stored
        width : the width of the resized image
        size : [width, hight], size of the resized image
    */
    Size imgSize = image.size();
    double r = width * 1.0 / imgSize.width;
    new_size.width = width;
    new_size.height = int(imgSize.height * r);
    resize(image, resized, new_size);
}

Mat clip(Mat& image, Size& size, dlib::rectangle rect) {
    /*
    Clip the frame and return the face region
    Args :
        img: a Mat, the image
        clipped: a Mat, the clipped image;
        size : size of the image when performing detection
        rect : an instance of dlib::rectangle, the face region
    */
    Size imSize = image.size();
    int left = int(1.0 * rect.left() / size.width * imSize.width);
    int right = int(1.0 * rect.right() / size.width * imSize.width);
    int top = int(1.0 * rect.top() / size.height * imSize.height);
    int bottom = int(1.0 * rect.bottom() / size.height * imSize.height);
    Rect imRect(left, top, right - left, bottom - top);
    return image(imRect);
}

class Detector {
private:
    double roiRatio = 5;
    double smoothRatio = 0.9;
    dlib::frontal_face_detector detector;
    dlib::shape_predictor predictor;
    dlib::rectangle* rect = nullptr;
    double marks_x[68] = { 0 };
    double marks_y[68] = { 0 };

public:
    Detector();

    Scalar detect(Mat& image);

    double dist(dlib::rectangle* rect1, dlib::rectangle* rect2);
};

Detector::Detector() {
    detector = get_frontal_face_detector();
    deserialize("../dep/shape_predictor_68_face_landmarks.dat") >> predictor;
}

Scalar Detector::detect(Mat& image) {
    /*Detect the face region and returns the ROI value
    Face detection is the slowest part.
    Args:
        image: a matrix<rgb_pixel>, the image
    Return :
        val: an array of ROI value in each color channel
    */

    system_clock::time_point ctime = system_clock::now();
    duration<double> time_span;

    // Resize the image to limit the calculation
    Size detectionSize = Size();
    Mat resized;
    myResize(image, resized, detectionSize, 500);

    // Perform face detection on a grayscale image
    Mat gray;
    cvtColor(resized, gray, CV_RGB2GRAY);

    //Convert the image from the cv format (Mat) to the dlib format (matrix<>)
    matrix<uchar> gray_dlib;
    assign_image(gray_dlib, cv_image<uchar>(gray));


    time_span = duration_cast<duration<double>>(system_clock::now() - ctime);
    cout << time_span.count() << " ";
    ctime = system_clock::now();


    // No need for upsample, because its effect is the same as resize
    std::vector<dlib::rectangle> rects = detector(gray_dlib);
    int num = rects.size();
    if (num == 0) {
        cout << "No face in the frame!" << endl;
        return Scalar();
    }
    if (num >= 2) {
        cout << "More than one face!" << endl;
        return Scalar();
    }

    dlib::rectangle* rect = &rects[0];
    
    //Perform landmark prediction on the face region
    Size clipSize = Size();
    Mat clipped = clip(image, detectionSize, *rect);
    myResize(clipped, clipped, clipSize, 500);
    matrix<rgb_pixel> clipped_dlib;
    assign_image(clipped_dlib, cv_image<bgr_pixel>(clipped));


    time_span = duration_cast<duration<double>>(system_clock::now() - ctime);
    cout << time_span.count() << " ";
    ctime = system_clock::now();


    dlib::rectangle re(0, 0, clipSize.width, clipSize.height);
    auto shape = predictor(clipped_dlib, \
        dlib::rectangle(0, 0, clipSize.width, clipSize.height));


    time_span = duration_cast<duration<double>>(system_clock::now() - ctime);
    cout << time_span.count() << " ";
    ctime = system_clock::now();


    double marks_x[68] = { 0 };
    double marks_y[68] = { 0 };
    for (int i = 0; i < 68; i++) {
        point p = shape.part(i);
        marks_x[i] = p.x();
        marks_y[i] = p.y();
    }

    // If not the first image, perform landmark smoothing
    if (this->rect != nullptr)
        if (dist(this->rect, rect) < 0.3)
            for (int i = 0; i < 68; i++) {
                marks_x[i] = smoothRatio * this->marks_x[i] + \
                    (1 - smoothRatio) * marks_x[i];
                marks_y[i] = smoothRatio * this->marks_y[i] + \
                    (1 - smoothRatio) * marks_y[i];
            }

    int rank_left[3] = { 1, 3, 31 };
    Rect rect_left = np_to_rect(rank_left, marks_x, marks_y, roiRatio);
    Scalar val_left = mean(clipped(rect_left));
    int rank_right[3] = { 13, 15, 35 };
    Rect rect_right = np_to_rect(rank_right, marks_x, marks_y, roiRatio);
    Scalar val_right = mean(clipped(rect_right));

    this->rect = rect;
    memcpy(this->marks_x, marks_x, 68 * sizeof(double));
    memcpy(this->marks_y, marks_y, 68 * sizeof(double));


    time_span = duration_cast<duration<double>>(system_clock::now() - ctime);
    cout << time_span.count() << " ";
    ctime = system_clock::now();


    return (val_left + val_right)/2;
}

double Detector::dist(dlib::rectangle * rect1, dlib::rectangle * rect2) {
    double distx = abs(rect1->left() - rect2->left()) \
        + abs(rect1->right() - rect2->right());
    double disty = abs(rect1->top() - rect2->top()) \
        + abs(rect1->bottom() - rect2->bottom());
    return abs(distx / (rect1->right() - rect1->left())) \
        + abs(disty / (rect1->bottom() - rect1->top()));
}


int main() {
//  freopen("output.txt", "w", stdout); // redirect stdout to the text file

    string videoPath = "../data/video/PIC_0430_clip.mp4";
    double startTime = 1;

    Detector detector;
    VideoCapture video(videoPath);
    int len = video.get(CV_CAP_PROP_FRAME_COUNT);
    double fps = video.get(CV_CAP_PROP_FPS);
    video.set(CV_CAP_PROP_POS_FRAMES, startTime * fps);
    len = int(len - startTime * fps);
    
    // Handle frame one by one
    double* time = new double[len];
    double* dat[3];
    for (int i = 0; i < 3; i++)
        dat[i] = new double[len];
    double t = 0;
    int cnt = 0;
    Mat frame;
    system_clock::time_point ctime;
    duration<double> time_span;
    while (video.isOpened()) {
        if (!video.read(frame))
            break;
        cnt++;
        ctime = system_clock::now();

        // cvNamedWindow("E1", CV_WINDOW_AUTOSIZE);
        // imshow("E1", frame);
        // if (char(cvWaitKey(1)) == 'q')
        //  break;

        // Detect
        Scalar value = detector.detect(frame);

        // Save results
        time[cnt] = t;
        for (int i = 0; i < 3; i++)
            dat[i][cnt] = value.val[i];

        time_span = duration_cast<duration<double>>(system_clock::now() - ctime);
        printf("%.2f\t%.3f\t%.3f\t%.3f\t%.1f\t%.2f\n", t, value[0], value[1],
            value[2], fps, 1 / time_span.count());

        t += 1 / fps;
    }

    video.release();
    destroyAllWindows();
    fclose(stdout);

    return 0;
}
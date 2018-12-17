/* 
 * PPG Viewer
 * Copyright (C) 2018 PPG Hou <https://github.com/ppghou>
 */
#include "ppgDetector.h"

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
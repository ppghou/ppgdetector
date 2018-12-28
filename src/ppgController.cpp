#include "ppgController.h"

Controller::Controller()
{
    m_playMode = 1;
    m_exitState = false;
}

//this function will run in new thread when "start()" is called.
void Controller::run()
{
    // qDebug()<<"start thread:"<<QThread::currentThreadId();

    string videoPath = "PIC_0427_clip.mp4";
    double startTime = 1;

    // read video file
    VideoCapture video(videoPath);
    int len = video.get(CV_CAP_PROP_FRAME_COUNT);
    double fps = video.get(CV_CAP_PROP_FPS);

    m_fps = fps;
    video.set(CV_CAP_PROP_POS_FRAMES, startTime * fps);
    len = int(len - startTime * fps);

    Filter filter(fps);
    Detector detector;

    // Handle frame one by one
    m_time = 0;
    int cnt = 0;
    Mat frame;
    system_clock::time_point ctime;
    duration<double> time_span;

    // read camera
    CvCapture* pCaputer = cvCreateCameraCapture(-1);

    while(1)
    {
        if(m_exitState){ // when m_exitState is set to true, break the loop.
            break;
        }
        if(m_playMode == 2){ // read file
            if (!video.read(frame))
               break;
        }
        else if(m_playMode == 1){ // read camera
            IplImage* pFrame = cvQueryFrame(pCaputer);
            if(!pFrame){
                break;
            }
            frame = cvarrToMat(pFrame);
        }
        cnt++;
        ctime = system_clock::now();

        // Detect: show frame in label as picture
        Scalar value = detector.detect(frame);
        Mat rgb;
        cvtColor(frame, rgb, CV_BGR2RGB);
        QImage img = QImage((const uchar*)(rgb.data), rgb.cols, rgb.rows,
                            rgb.cols * rgb.channels(), QImage::Format_RGB888);

        emit signal_showImage(img);

        // Filter
        filter.filterInput(value.val[1]);
        emit signal_returnPPGValue(m_time, 255-value.val[1]);

        std::vector<int> peak_pos = filter.getPeakPos();
        std::vector<double> hr_val = filter.getFrequency();
        filter.clearTmpData();
        for(int i = 0; i < hr_val.size(); i++){
            emit signal_returnHRValue(double(peak_pos[i])/fps, hr_val[i]);
        }

        // Save results
        time_span = duration_cast<duration<double>>(system_clock::now() - ctime); /* DevSkim: ignore DS154189 */
        printf("%.2f\t%.3f\t%.3f\t%.3f\t%.1f\t%.2f\n", m_time, value[0], value[1],
                value[2], fps, 1 / time_span.count());
        m_time += 1 / fps;
    }

    detector.initialize();
    video.release();
    fclose(stdout);
}

void Controller::slot_setPlayingState(bool state)
{
    printf("slot_setPlayingState\n"); /* DevSkim: ignore DS154189 */
    m_exitState = state;
}

void Controller::slot_setVideoMode(int mode)
{
    m_playMode = mode;
}

double Controller::getFps()
{
    return m_fps;
}

double Controller::getCalcTime()
{
    return m_time;
}

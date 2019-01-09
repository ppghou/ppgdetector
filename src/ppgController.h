#ifndef __CONTROLLER__
#define __CONTROLLER__

#include <QThread>
#include <QImage>
#include <QDebug>
#include "ppgDetector.h"
#include "ppgFilter.h"

#define VIDEO_MODE_CAMERA 0
#define VIDEO_MODE_FILE   1

class Controller : public QThread
{
    Q_OBJECT
public:
    Controller(QString videoPath);
    double getFps();
    double getCalcTime();
signals:
    void signal_showImage(QImage img); // emit when read a new frame
    void signal_returnPPGValue(double time, double value, int line); // emit when caculated a frame
    void signal_returnHRValue(double time, double value);
public slots:
    void slot_setVideoMode(int mode);
    void slot_setVideoPath(QString newVideoPath);
    void slot_setPlayingState(bool state);
    void slot_saveData(QString saveDataPath);
protected:
    void run(); // when DetectThread.start() called, run function "run()".
private:
    int m_playMode;
    bool m_exitState;
    double m_time;
    double m_fps;
    std::vector<double> m_ppgTime;
    std::vector<double> m_ppgValue;
    QString m_videoPath;
};

#endif // __CONTROLLER__

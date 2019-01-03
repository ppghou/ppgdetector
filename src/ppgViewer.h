#ifndef __PPG_VIEWER__
#define __PPG_VIEWER__

#include <QDebug>
#include <QLineSeries>
#include <QFileDialog>
#include <QMainWindow>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>

#include "ppgController.h"

using namespace std;

namespace Ui {
class ppgViewer;
}

class ppgViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ppgViewer(QWidget *parent = 0);
    ~ppgViewer();
    QLineSeries *m_ppgSeries;
    QLineSeries *m_hrSeries;
    void initialize();
private slots:
    void slot_showImage(QImage img);
    void slot_returnPPGValue(double time, double value);
    void slot_returnHRValue(double time, double value);
    void on_pushButton_save_clicked();
    void on_pushButton_start_clicked();
    void on_pushButton_videoMode_clicked();
signals:
    void signal_setVideoMode(int mode);
    void signal_setVideoPath(QString newVideoPath);
    void signal_setPlayingState(bool state);
    void signal_saveData(QString saveDataPath);
private:
    Ui::ppgViewer *ui;

    Controller* m_controller;
    QChartView* m_ppgChartView;
    QChartView* m_hrChartView;
    
    int m_videoMode;
    bool m_isPlaying;
    bool m_initState;
    double m_fps;
    double m_ppgTimeSpan;
    double m_hrTimeSpan;
    QString m_videoPath;

    QChart *createPPGChart();
    QChart *createHRChart();
};

#endif // __PPG_VIEWER__

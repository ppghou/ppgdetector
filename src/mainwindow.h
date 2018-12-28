#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QLineSeries>
#include <QtCharts/QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QAbstractBarSeries>
#include <QtCharts/QPercentBarSeries>
#include <QtCharts/QStackedBarSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QLegend>
#include <QtCharts/QValueAxis>

#include "ppgController.h"

typedef QPair<QPointF, QString> Data;
typedef QList<Data> DataList;
typedef QList<DataList> DataTable;

using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QLineSeries *m_ppgSeries;
    QLineSeries *m_hrSeries;
    void createCharts();
private slots:
    void slot_showImage(QImage img);
    void slot_returnPPGValue(double time, double value);
    void slot_returnHRValue(double time, double value);
    void on_pushButton_start_clicked();
    void on_pushButton_videoMode_clicked();
signals:
    void signal_setVideoMode(int mode);
    void signal_setPlayingState(bool state);
private:
    double m_ppgTimeSpan;
    double m_hrTimeSpan;
    Ui::MainWindow *ui;
    Controller* m_controller;
    QChartView* m_ppgChartView;
    QChartView* m_hrChartView;
    DataTable m_dataTable;
    int m_videoMode;    //1:camera 2:file
    bool m_isPlaying;
    double FPS;
    QChart *createPPGChart();
    QChart *createHRChart();
    DataTable generateRandomData(int listCount, int valueMax, int valueCount) const;
};

#endif // MAINWINDOW_H

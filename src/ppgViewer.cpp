#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>

#include <VLCQtCore/Common.h>
#include <VLCQtCore/Instance.h>
#include <VLCQtCore/Media.h>
#include <VLCQtCore/MediaPlayer.h>

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "ppgViewer.h"
#include "ui_ppgViewer.h"
#include <fstream>

using namespace std;
QT_CHARTS_USE_NAMESPACE

ppgViewer::ppgViewer(QWidget *parent): QMainWindow(parent), ui(new Ui::ppgViewer), _media(0)
{
    ui->setupUi(this);
    
    _instance = new VlcInstance(VlcCommon::args(), this);
    _player = new VlcMediaPlayer(_instance);
    _player->setVideoWidget(ui->video);

    ui->video->setMediaPlayer(_player);
    ui->volume->setMediaPlayer(_player);
    ui->volume->setVolume(50);
    ui->seek->setMediaPlayer(_player);

    // lastFile = Q_NULLPTR;
    
    connect(ui->actionOpenLocal, &QAction::triggered, this, &ppgViewer::openLocal);
    connect(ui->actionOpenUrl, &QAction::triggered, this, &ppgViewer::openUrl);
    connect(ui->actionPause, &QAction::toggled, _player, &VlcMediaPlayer::togglePause);
    connect(ui->actionStop, &QAction::triggered, _player, &VlcMediaPlayer::stop);
    
    connect(ui->openLocal, &QPushButton::clicked, this, &ppgViewer::openLocal);
    connect(ui->openUrl, &QPushButton::clicked, this, &ppgViewer::openUrl);
    connect(ui->pause, &QPushButton::toggled, ui->actionPause, &QAction::toggle);
    connect(ui->stop, &QPushButton::clicked, _player, &VlcMediaPlayer::stop);

    // setupHeartRateChart();
    // setupElectrocarChart();
}

ppgViewer::~ppgViewer()
{
    delete _player;
    delete _media;
    delete _instance;
    delete ui;
}

// 弹窗选择视频
void ppgViewer::openLocal()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("选择需要解析的视频"),
                            QDir::homePath(),
                            tr("Multimedia files(*)"));
    
    if (fileName.isEmpty()) return;

    _media = new VlcMedia(fileName, true, _instance);
    _player->open(_media);
    // lastFile = fileName;
    
    qDebug() << fileName;
}

// 选择 Url 地址
void ppgViewer::openUrl()
{
    QString url =
            QInputDialog::getText(this, tr("Open Url"), tr("Enter the URL you want to play"));

    if (url.isEmpty())
        return;

    _media = new VlcMedia(url, _instance);

    _player->open(_media);
}

//初始化心率图
// void Viewer::setupHeartRateChart()
// {
//     QValueAxis *axisX = new QValueAxis;
//     axisX->setRange(0, 10);           //设置范围
//     axisX->setLabelFormat("%.1lf");   //设置刻度的格式
//     axisX->setGridLineVisible(true);  //网格线可见
//     //axisX->setTickCount(10);        //设置多少格
//     axisX->setMinorTickCount(1);      //设置每格小刻度线的数目

//     QValueAxis *axisY = new QValueAxis;
//     axisY->setRange(-10, 10);
//     axisY->setLabelFormat("%.1lf");
//     axisY->setGridLineVisible(true);
//     //axisY->setTickCount(10);
//     axisY->setMinorTickCount(1);

//     QChart * HeartRateChart = new QChart();

//     QLineSeries *series = new QLineSeries();

//     series->append(0, 6);
//     series->append(2, 4);
//     series->append(3, 8);
//     series->append(7, 4);
//     series->append(10, 5);

//     HeartRateChart->addSeries(series);
//     HeartRateChart->setAxisX(axisX, series);
//     HeartRateChart->setAxisY(axisY, series);
//     HeartRateChart->setTitle("Heart Rate");
//     HeartRateChart->legend()->hide();

//     ui->heartRateChart->setRenderHint(QPainter::Antialiasing);
//     ui->heartRateChart->setChart(HeartRateChart);
// }

// void Viewer::setupElectrocarChart()
// {
//     QValueAxis *axisX = new QValueAxis;
//     axisX->setRange(0, 10);          //设置范围
//     axisX->setLabelFormat("%.1lf");  //设置刻度的格式
//     axisX->setGridLineVisible(true); //网格线可见
//     //axisX->setTickCount(10);       //设置多少格
//     axisX->setMinorTickCount(1);     //设置每格小刻度线的数目

//     QValueAxis *axisY = new QValueAxis;
//     axisY->setRange(-10, 10);
//     axisY->setLabelFormat("%.1lf");
//     axisY->setGridLineVisible(true);
//     //axisY->setTickCount(10);
//     axisY->setMinorTickCount(1);

//     QChart * ElectrocarChart = new QChart();

//     QLineSeries *series = new QLineSeries();

//     series->append(0, 6);
//     series->append(2, 4);
//     series->append(3, 8);
//     series->append(7, 4);
//     series->append(10, 5);

//     ElectrocarChart->addSeries(series);
//     ElectrocarChart->setAxisX(axisX, series);
//     ElectrocarChart->setAxisY(axisY, series);
//     ElectrocarChart->setTitle("Electrocar Diagram");
//     ElectrocarChart->legend()->hide();

//     ui->electrocarChart->setRenderHint(QPainter::Antialiasing);
//     ui->electrocarChart->setChart(ElectrocarChart);
// }

// void updateChart(QChart * oldChart, QLineSeries * series, double Xmin, double Xmax, double Ymin, double Ymax)
// {
//     oldChart->removeAllSeries();
//     oldChart->addSeries(series);
//     oldChart->axisX()->setMin(Xmin);
//     oldChart->axisX()->setMax(Xmax);
//     oldChart->axisY()->setMin(Ymin);
//     oldChart->axisY()->setMax(Ymax);
//     oldChart->setAxisX(oldChart->axisX(), series);
//     oldChart->setAxisY(oldChart->axisY(), series);
// }

// void Viewer::on_start_clicked()
// {
//     /*if(lastFile == Q_NULLPTR)
//         return;
//     ifstream input( lastFile.toStdString(), ios::in );
//     QLineSeries *series = new QLineSeries();
//     double Xmin = 0, Ymin = 0, Xmax = 0, Ymax = 0;
//     double x = 0, y = 0;
//     while(!input.eof())
//     {
//         input >> x >> y;
//         if(x < Xmin)
//             Xmin = x;
//         if(x > Xmax)
//             Xmax = x;
//         if(y < Ymin)
//             Ymin = y;
//         if(y > Ymax)
//             Ymax = y;
//         series->append(x, y);
//     }
//     //updateChart(ui->electrocarChart->chart(), series, Xmin, Xmax, Ymin, Ymax);
//     updateChart(ui->heartRateChart->chart(), series, Xmin, Xmax, Ymin, Ymax);*/
//     libvlc_instance_t * inst;
//     libvlc_media_player_t *mp;
//     libvlc_media_t *m;

//     libvlc_time_t length;
//     int width;
//     int height;
//     int wait_time=5000;
//     inst = libvlc_new (0, nullptr);

//     m = libvlc_media_new_path (inst, "aaa.mkv");
//           Create a media player playing environement 
//          mp = libvlc_media_player_new_from_media (m);


//          /* No need to keep the media now */
//          libvlc_media_release (m);
//          libvlc_media_player_set_hwnd (mp,(void*)ui->videoPlayer->winId());

//          // play the media_player
//          //libvlc_media_player_set_rate(mp, 5.0);
//          libvlc_media_player_play (mp);
//          libvlc_media_player_set_time(mp, 6000000);
//          //wait until the tracks are created
//          Sleep (wait_time);
//          length = libvlc_media_player_get_length(mp);
//          width = libvlc_video_get_width(mp);
//          height = libvlc_video_get_height(mp);
//          qDebug() << "Stream Duration: " << length/1000 << "s\n";
//          qDebug() << "Resolution: " << width << " x " << height << "\n";
//          //Let it play
//          //Sleep (length);

//          // Stop playing
//          //libvlc_media_player_stop (mp);

//          // Free the media_player
//          //libvlc_media_player_release (mp);

//          //libvlc_release (inst);
// }
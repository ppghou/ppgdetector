// #include "mainwindow.h"
// #include "ui_mainwindow.h"
// #include "QDebug"
// #include "QFileDialog"
// #include "QtCharts/QChartView"
// #include "QtCharts/QLineSeries"
// #include "QtCharts/QValueAxis"
// #include <fstream>
// using namespace std;

// QT_CHARTS_USE_NAMESPACE

// MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
// {
//     ui->setupUi(this);
//     lastFile = Q_NULLPTR;
//     setupHeartRateChart();
//     setupElectrocarChart();
// }

// MainWindow::~MainWindow()
// {
//     delete ui;
// }

// // 弹窗选择视频
// void MainWindow::on_selectFile_clicked()
// {
//     QString file_name = QFileDialog::getOpenFileName(Q_NULLPTR,"选择需要解析的视频",lastFile,"*.txt");
//     lastFile = file_name;
//     qDebug() << file_name;
// }

// //初始化心率图
// void MainWindow::setupHeartRateChart()
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

// void MainWindow::setupElectrocarChart()
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

// void MainWindow::on_start_clicked()
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
//          /* Create a media player playing environement */
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

#include <vlc/vlc.h>
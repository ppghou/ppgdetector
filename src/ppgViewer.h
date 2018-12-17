/* 
 * PPG Viewer
 * Copyright (C) 2018 PPG Hou <https://github.com/ppghou>
 */
#ifndef ppgViewer_H
#define ppgViewer_H

#include <QMainWindow>

namespace Ui {
    class ppgViewer;
}

class VlcInstance;
class VlcMedia;
class VlcMediaPlayer;

class ppgViewer : public QMainWindow
{
    Q_OBJECT

public:
    explicit ppgViewer(QWidget *parent = 0);
    ~ppgViewer();

private slots:
    void openLocal();
    void openUrl();
    // void setupHeartRateChart();
    // void setupElectrocarChart();
    // void on_start_clicked();
    // void updateChart(QChart * oldChart, QLineSeries * series, int Xmin, int Xmax, int Ymin, int Ymax)

private:
    Ui::ppgViewer *ui;

    VlcInstance *_instance;
    VlcMedia *_media;
    VlcMediaPlayer *_player;

    QString lastFile;
};

#endif // ppgViewer_H

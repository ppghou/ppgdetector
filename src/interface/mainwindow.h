#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>

extern "C"
{
   #include <vlc/vlc.h>
}

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_selectFile_clicked();
    void setupHeartRateChart();
    void setupElectrocarChart();
    void on_start_clicked();
    //void updateChart(QChart * oldChart, QLineSeries * series, int Xmin, int Xmax, int Ymin, int Ymax)

private:
    Ui::MainWindow *ui;
    QString lastFile;
};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_controller = new Controller();
    connect(m_controller, SIGNAL(signal_showImage(QImage)), this,
                            SLOT(slot_showImage(QImage)));
    connect(m_controller, SIGNAL(signal_returnPPGValue(double,double)), this,
                            SLOT(slot_returnPPGValue(double,double)));
    connect(m_controller, SIGNAL(signal_returnHRValue(double,double)), this,
                            SLOT(slot_returnHRValue(double,double)));
    connect(this, SIGNAL(signal_setPlayingState(bool)), m_controller,
                  SLOT(slot_setPlayingState(bool)));
    connect(this, SIGNAL(signal_setVideoMode(int)), m_controller,
                  SLOT(slot_setVideoMode(int)));
    m_videoMode = 1;
    m_ppgTimeSpan = 10;
    m_hrTimeSpan = 30;
    m_isPlaying = false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

//show frame in window
void MainWindow::slot_showImage(QImage img)
{
    ui->label->clear();
    ui->label->setScaledContents(true);
    ui->label->setPixmap(QPixmap::fromImage(img));
}

//get a new PPG data, refresh the ppg chart
void MainWindow::slot_returnPPGValue(double time, double value)
{
    // qDebug() << "in slot_returnPPGValue";
    // qDebug() << "value is " << value;
    FPS = m_controller->getFps();
    QVector<QPointF> data = m_ppgSeries->pointsVector();
    data.append(QPointF(time, value));
    m_ppgSeries->append(QPointF(time, value));

    int beg = int(((time-m_ppgTimeSpan)>0?(time-m_ppgTimeSpan):0)*FPS);
    // cout << "beg: " << beg;
    double max = data.at(beg).y();
    double min = data.at(beg).y();
    for(int i = beg+1;i < data.size()-1; ++i){
        double val = data.at(i).y();
        max = val > max ? val : max;
        min = val < min ? val : min;
    }
    m_ppgChartView->chart()->axisY()->setRange(min-1, max+1);

    if(time<m_ppgTimeSpan)
        m_ppgChartView->chart()->axisX()->setRange(0, m_ppgTimeSpan);
    else
        m_ppgChartView->chart()->axisX()->setRange(time-m_ppgTimeSpan, time);
}

//get a new heart rate data, refresh the hr chart
void MainWindow::slot_returnHRValue(double time, double value)
{
    QVector<QPointF> data = m_hrSeries->pointsVector();
    data.append(QPointF(time, value));

    m_hrChartView->chart()->axisY()->setRange(50, 120);
    if(time<m_hrTimeSpan)
        m_hrChartView->chart()->axisX()->setRange(0, m_hrTimeSpan);
    else
        m_hrChartView->chart()->axisX()->setRange(time-m_hrTimeSpan, time);
    m_hrSeries->append(QPointF(time, value));
}

QChart *MainWindow::createPPGChart()
{
    // qDebug() << "in createPPGChart";
    QChart *chart = new QChart();
    QString name("PPG ");

    m_ppgSeries = new QLineSeries(chart);
    m_ppgSeries->setName(name);
    chart->addSeries(m_ppgSeries);
    chart->createDefaultAxes();

    // Add space to label to add space between labels and axis
    static_cast<QValueAxis *>(chart->axisY())->setLabelFormat("%.1f  ");

    return chart;
}

QChart* MainWindow::createHRChart()
{
    // Debug() << "in createPPGChart";
    QChart *chart = new QChart();
    QString name("HR ");

    m_hrSeries = new QLineSeries(chart);
    m_hrSeries->setName(name);
    chart->addSeries(m_hrSeries);
    chart->createDefaultAxes();

    // Add space to label to add space between labels and axis
    static_cast<QValueAxis *>(chart->axisY())->setLabelFormat("%.1f  ");

    return chart;
}

void MainWindow::createCharts()
{
    // qDebug() << "in create charts ";
    m_ppgChartView = new QChartView(createPPGChart());
    m_hrChartView = new QChartView(createHRChart());
    ui->horizontalLayout->addWidget(m_ppgChartView, 0);//add ppg chart to window
    ui->horizontalLayout->addWidget(m_hrChartView, 1);//add hr chart to window
}

void MainWindow::on_pushButton_start_clicked()
{
    if(m_isPlaying) //if is playing , stop it.
    {
        m_isPlaying = false;
        ui->pushButton_start->setText("start");
        emit signal_setPlayingState(true);
        // m_controller->quit();
    }
    else //if is not playing, start it.
    {
        m_isPlaying = true;
        ui->pushButton_start->setText("stop");
        emit signal_setPlayingState(false);
        m_controller->start();
    }
}

//change video mode when clicked
void MainWindow::on_pushButton_videoMode_clicked()
{
    if(m_videoMode == 1)
    {
        m_videoMode = 2;
        ui->pushButton_videoMode->setText("File");
        emit signal_setVideoMode(m_videoMode);
    }
    else if(m_videoMode == 2)
    {
        m_videoMode = 1;
        ui->pushButton_videoMode->setText("Camera");
        emit signal_setVideoMode(m_videoMode);
    }
}

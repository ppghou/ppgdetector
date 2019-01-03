#include "ppgViewer.h"
#include "ui_ppgViewer.h"

ppgViewer::ppgViewer(QWidget *parent): QMainWindow(parent), ui(new Ui::ppgViewer)
{
    ui->setupUi(this);
    m_videoPath = "../data/video/PIC_0427.mp4";
    m_controller = new Controller(m_videoPath);
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
    connect(this, SIGNAL(signal_setVideoPath(QString)), m_controller,
                  SLOT(slot_setVideoPath(QString)));
    connect(this, SIGNAL(signal_saveData(QString)), m_controller,
                  SLOT(slot_saveData(QString)));
    m_videoMode = VIDEO_MODE_CAMERA;
    m_ppgTimeSpan = 10;
    m_hrTimeSpan = 30;
    m_isPlaying = false;
    m_initState = true;
}

ppgViewer::~ppgViewer()
{
    delete ui;
}

// SLOT: show frame in window
void ppgViewer::slot_showImage(QImage img)
{
    if(m_isPlaying){
        ui->label->clear();
        ui->label->setScaledContents(true);
        ui->label->setPixmap(QPixmap::fromImage(img));
    }
}

//SLOT: get a new PPG data, refresh the ppg chart
void ppgViewer::slot_returnPPGValue(double time, double value)
{
    // qDebug() << "in slot_returnPPGValue";
    // qDebug() << "value is " << value;
    m_fps = m_controller->getFps();
    QVector<QPointF> data = m_ppgSeries->pointsVector();
    data.append(QPointF(time, value));
    m_ppgSeries->append(QPointF(time, value));

    int beg = int(((time-m_ppgTimeSpan)>0?(time-m_ppgTimeSpan):0)*m_fps);
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

// SLOT: get a new heart rate data, refresh the hr chart
void ppgViewer::slot_returnHRValue(double time, double value)
{
    QVector<QPointF> data = m_hrSeries->pointsVector();
    data.append(QPointF(time, value));
    m_hrSeries->append(QPointF(time, value));
    m_hrChartView->chart()->axisY()->setRange(50, 120);

    if(time<m_hrTimeSpan)
        m_hrChartView->chart()->axisX()->setRange(0, m_hrTimeSpan);
    else
        m_hrChartView->chart()->axisX()->setRange(time-m_hrTimeSpan, time);
}

// FUNC: create ppg chart
QChart *ppgViewer::createPPGChart()
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

// FUNC: create heart rate chart
QChart* ppgViewer::createHRChart()
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

// FUNC: initialize
void ppgViewer::initialize()
{
    // qDebug() << "in create charts ";
    ui->ppgChartView->setChart(createPPGChart());//add ppg chart to window
    ui->hrChartView->setChart(createHRChart());//add hr chart to window
    m_ppgChartView = ui->ppgChartView;
    m_hrChartView = ui->hrChartView;
    m_controller->start();
}

// FUNC: pushButton_start slot, handle start
void ppgViewer::on_pushButton_save_clicked()
{
    if(m_isPlaying) on_pushButton_start_clicked();
    if(!m_initState){
        QString saveDataPath = QFileDialog::getOpenFileName(this, tr("输入需要保存的文件名"),
                            QDir::homePath(), tr("*.csv"));
        if (!saveDataPath.isEmpty())
            emit signal_saveData(saveDataPath);
    }
    
}

// FUNC: pushButton_start slot, handle start
void ppgViewer::on_pushButton_start_clicked()
{
    // if is playing , stop it.
    if(m_isPlaying){
        m_isPlaying = false;
        ui->pushButton_start->setText("Start");
        emit signal_setPlayingState(m_isPlaying);
    }
    // if is not playing, start it.
    else{
        m_isPlaying = true;
        ui->pushButton_start->setText("Stop");
        emit signal_setPlayingState(m_isPlaying);
        m_ppgSeries->clear();
        m_hrSeries->clear();
        m_initState = false;
    }
}

// FUNC: pushButton_videoMode slot, change video mode when clicked
void ppgViewer::on_pushButton_videoMode_clicked()
{
    if(!m_isPlaying){
        if(m_videoMode == VIDEO_MODE_CAMERA){
            m_videoMode = VIDEO_MODE_FILE;
            ui->pushButton_videoMode->setText("File");
            emit signal_setVideoMode(m_videoMode);

            QString videoPath = QFileDialog::getOpenFileName(this, tr("选择需要解析的视频"),
                                QDir::homePath(), tr("Multimedia files(*)"));
            if (videoPath.isEmpty()) return;
            else emit signal_setVideoPath(videoPath);
        }
        else if(m_videoMode == VIDEO_MODE_FILE){
            m_videoMode = VIDEO_MODE_CAMERA;
            ui->pushButton_videoMode->setText("Camera");
            emit signal_setVideoMode(m_videoMode);
        }
    }
}

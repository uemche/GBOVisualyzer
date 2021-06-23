#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"
#include "QMessageBox"
#include "QImage"
#include "QImageReader"
#include "QLabel"
#include "QPainter"
#include "QMouseEvent"
#include "QMessageBox"
#include "time.h"
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString gboName;
    QString idxName;
    QImage img;
    QImage imgOut;
    QMouseEvent *event;
    QPoint Cursor;
    QMessageBox ErrorMsg;
    int rowsCount;
    int colsCount;
    int wheight;
    int wwidth;
    int iheight;
    int iwidth;
    QCPItemTracer *reliefTracer;
    QCPItemTracer *trekTracer;
    QVector<double> rows;
    QVector<double> depths;
    QVector<double> heights;
    QVector<double> lats;
    QVector<double> lons;
    QVector<double> spacing;
    #pragma pack(push, 4)
    struct idx_header{
        short verMajor;
        short verMinor;
        int data_pointer;
        int varu_pointer;
        int spl;
        int sps;
        short resol;
        short compress;
        time_t time;
        unsigned short tzi;
        char reserved[46];
        unsigned int total_str;
        int crc32;
    };
    #pragma pack(pop)
    #pragma pack(push, 4)
    struct idx_data{
        unsigned int num_str;
        time_t time;
        int tauzi;
        float heading;
        float roll;
        float pitch;
        float speed;
        float vspeed;
        double lat;
        double lon;
        float alt;
        float depth;
        char reserved[73];
    };
    #pragma pack(pop)
    idx_data IdxData;
    idx_header IdxHeader;
    FILE *IdxFile;
    uchar buffer[100];
    int hp;
private slots:
    void on_actionOpen_triggered();
    void on_tableWidget_cellClicked(int row);

private:
    Ui::MainWindow *ui;
    void loadImage(const QString &);
    void mouseEvent(QMouseEvent *);
    void loadData();
    void drawRelief(QVector<double> y1, QVector<double> y2, QVector<double> x, int N);
    void drawTrek(QVector<double> x, QVector<double> y);
    void drawHist(int line);
    void reliefEvent(QMouseEvent *);
protected:
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
};
#endif // MAINWINDOW_H

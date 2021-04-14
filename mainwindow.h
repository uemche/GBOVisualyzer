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
    QMouseEvent *event;
    QPoint localCursor;
    QMessageBox ErrorMsg;
    QPixmap frame;
    int wheight;
    int wwidth;
    int iheight;
    int iwidth;
    int frameNum;
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
    FILE *IdxFile = NULL;
    uchar buffer[100];
    int hp;
private slots:
    void on_actionOpen_2_triggered();
    void loadFile(const QString &);
    void mousePressEvent(QMouseEvent *);

    void on_prevFrame_pressed();

    void on_nextFrame_pressed();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "QMouseEvent"
#include <stdio.h>
#include "math.h"
#include <bitset>
#include "QColor"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    gboName = "";
    idxName = "";
    wheight = 768;
    wwidth = 1024;
    iheight = wheight - 80;
    iwidth = wwidth/2;
    frameNum = 0;
    ui->setupUi(this);
    setFixedSize(wwidth, wheight);
    ui->label->setGeometry(0,20,iwidth,iheight);
    ui->label->setPixmap(QPixmap(iwidth, iheight));
}

MainWindow::~MainWindow()
{
    fclose(IdxFile);
    delete ui;
}


void MainWindow::on_actionOpen_2_triggered()
{
    try {
        std::cout<<frameNum<<std::endl;
        gboName = QFileDialog::getOpenFileName(this, tr("Open data"),"",tr("*.gbo"));
        idxName = gboName.replace(".gbo",".idx");
        gboName.replace(".idx", ".gbo");
        if (gboName == NULL or idxName == NULL){
            throw -1;
        }
        else if (fopen(idxName.toLocal8Bit().data(), "rb") == NULL){
            throw -2;
        }
        IdxFile = fopen(idxName.toLocal8Bit().data(), "rb");
        fseek(IdxFile,20,0);
        fread(buffer, 4, 1, IdxFile);
        hp = *((int *)buffer);
        fseek(IdxFile, hp, 0);
        fread(&IdxHeader, sizeof(idx_header), 1, IdxFile);
        loadFile(gboName);
    } catch (int a) {
        if (a == -2){
            std::cout<< "No file in derictory" << std::endl;
            ErrorMsg.setText("В папке отсутствует idx-файл.");
            ErrorMsg.exec();
            gboName = "";
            idxName = "";
        }
    }
}

void MainWindow::loadFile(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        QImage image(fileName);
        img = image;
        image = image.scaled(iwidth, image.height(), Qt::IgnoreAspectRatio);
        QRect rect(0, frameNum*iheight, iwidth, iheight);
        QImage copy = image.copy(rect);
        frame = frame.fromImage(copy);
        ui->label->setGeometry(0,20,iwidth,iheight);
        ui->label->setPixmap(frame);
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    loadFile(gboName);
    QPainter paint;
    if (event->button() == Qt::LeftButton) {
        localCursor = event->pos();
        try {
            if (IdxFile == NULL){
                throw -1;
            }
            if (localCursor.x()<iwidth && wheight-40 > localCursor.y() && localCursor.y() > 40){
                paint.begin(&frame);
                paint.drawLine(0, localCursor.y()-40, iwidth, localCursor.y()-40);
                paint.end();
                ui->label->setPixmap(frame);
                //IdxFile = fopen(idxName.toLocal8Bit().data(), "rb");
                fseek(IdxFile, IdxHeader.data_pointer, 0);
                fseek(IdxFile, sizeof(idx_data)*(localCursor.y() - 41)+frameNum*sizeof(idx_data)*(iheight-1), SEEK_CUR);
                fread(&IdxData, sizeof(idx_data), 1, IdxFile);
                if(feof(IdxFile)){
                    throw -2;
                }
                ui->line_num_str->setText(QString::number(IdxData.num_str));
                struct tm *t;
                t = localtime(&IdxData.time);
                char temp_time[100];
                std::strftime(temp_time, 100, "%a, %d.%m.%Y %H:%M:%S", t);
                ui->line_time->setText(QString(temp_time));
                ui->line_tauzi->setText(QString::number(IdxData.tauzi));
                ui->line_heading->setText(QString::number(IdxData.heading));
                ui->line_roll->setText(QString::number(IdxData.roll));
                ui->line_pitch->setText(QString::number(IdxData.pitch));
                ui->line_speed->setText(QString::number(IdxData.speed));
                ui->line_vspeed->setText(QString::number(IdxData.vspeed));
                ui->line_lat->setText(QString::number(IdxData.lat));
                ui->line_lon->setText(QString::number(IdxData.lon));
                ui->line_alt->setText(QString::number(IdxData.alt));
                ui->line_depth->setText(QString::number(IdxData.depth));
                ui->line_reserved->setText(QString::number(sizeof(IdxData.reserved)));
                //fclose(IdxFile);
            }
        } catch (int a) {
            if (a == -1){
                std::cout<< "Files aren't loaded yet" << std::endl;
            }
            else if (a == -2){
                std::cout << "No data here" << std::endl;
            }
        }
    }

}

void MainWindow::on_prevFrame_pressed()
{
    try {
        if(gboName == NULL){
            throw -1;
        }
        frameNum --;
        if(frameNum < 0){
            frameNum ++;
            throw -2;
        }
        loadFile(gboName);
    } catch (int a) {
        if (a == -1){
            ErrorMsg.setText("Файлы пока не загружены");
            ErrorMsg.exec();
        }
        else if(a == -2){
            ErrorMsg.setText("Вы на первом фрагменте.");
            ErrorMsg.exec();
        }

    }

}

void MainWindow::on_nextFrame_pressed()
{
    try {
        if(gboName == NULL){
            throw -1;
        }
        frameNum ++;
        if(frameNum > ceil(img.height()/iheight)){
            frameNum--;
            throw -2;
        }
        loadFile(gboName);
    } catch (int a) {
        if (a == -1){
            ErrorMsg.setText("Файлы пока не загружены");
            ErrorMsg.exec();
        }
        else if(a == -2){
            ErrorMsg.setText("Вы на последнем фрагменте.");
            ErrorMsg.exec();
        }
    }

}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "QMouseEvent"
#include <stdio.h>
#include "math.h"
#include <cmath>
#include <bitset>
#include "QColor"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    gboName = "";
    idxName = "";
    img = QImage(NULL);
    imgOut = QImage(NULL);
    IdxFile = NULL;
    ui->setupUi(this);
    setMouseTracking(true);
    connect(ui->reliefPlot, &QCustomPlot::mousePress, this, &MainWindow::reliefEvent);
    connect(ui->reliefPlot, &QCustomPlot::mouseMove, this, &MainWindow::reliefEvent);
    reliefTracer = new QCPItemTracer(ui->reliefPlot);
    trekTracer = new QCPItemTracer(ui->trekPlot);
    wwidth = this->width();
    wheight = this->height();
    iwidth = 460;
    ui->label->hide();
    ui->scrollArea->hide();
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->histoPlot->xAxis->setTicks(false);
    ui->histoPlot->yAxis->setTickLengthIn(0);
    ui->histoPlot->yAxis->setTickLengthOut(4);
    ui->histoPlot->xAxis->setTickLengthIn(0);
    ui->histoPlot->xAxis->setTickLengthOut(4);
    ui->histoPlot->yAxis->setSubTickLengthIn(0);
    ui->histoPlot->yAxis->setSubTickLengthOut(2);
    ui->histoPlot->yAxis->setRange(0, 255);
    ui->histoPlot->yAxis2->setOffset(20);
    ui->histoPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight);
    ui->trekPlot->yAxis->setSubTickLengthIn(0);
    ui->trekPlot->yAxis->setSubTickLengthOut(2);
    ui->trekPlot->xAxis->setSubTickLengthIn(0);
    ui->trekPlot->xAxis->setSubTickLengthOut(2);
    ui->trekPlot->yAxis->setTickLengthIn(0);
    ui->trekPlot->yAxis->setTickLengthOut(4);
    ui->trekPlot->xAxis->setTickLengthIn(0);
    ui->trekPlot->xAxis->setTickLengthOut(4);
    ui->reliefPlot->yAxis->setSubTickLengthIn(0);
    ui->reliefPlot->yAxis->setSubTickLengthOut(2);
    ui->reliefPlot->xAxis->setSubTickLengthIn(0);
    ui->reliefPlot->xAxis->setSubTickLengthOut(2);
    ui->reliefPlot->yAxis->setTickLengthIn(0);
    ui->reliefPlot->yAxis->setTickLengthOut(4);
    ui->reliefPlot->xAxis->setTickLengthIn(0);
    ui->reliefPlot->xAxis->setTickLengthOut(4);
    colsCount = ui->tableWidget->columnCount();
}
MainWindow::~MainWindow()
{
    if(IdxFile != NULL)
        fclose(IdxFile);
    delete ui;
}


void MainWindow::on_actionOpen_2_triggered()
{
    try {
        gboName = QFileDialog::getOpenFileName(this, "Open data","","*.gbo");
        idxName = gboName.replace(".gbo",".idx");
        gboName.replace(".idx", ".gbo");
        if (gboName == "" or idxName == ""){
            throw -1;
        }
        else if ((IdxFile = fopen(idxName.toLocal8Bit().data(), "rb")) == NULL){
            throw -2;
        }
        loadData();
        loadImage(gboName);
    } catch (int a) {
        gboName = "";
        idxName = "";
        img = QImage(NULL);
        imgOut = QImage(NULL);
        IdxFile = NULL;
        ui->tableWidget->clearContents();
        ui->reliefPlot->clearGraphs();
        ui->trekPlot->clearGraphs();
        ui->histoPlot->clearGraphs();
        ui->reliefPlot->axisRect()->setBackground(Qt::white);
        ui->reliefPlot->replot();
        ui->trekPlot->replot();
        ui->histoPlot->replot();
        ui->label->hide();
        ui->scrollArea->hide();
        ui->timeCreate->clear();
        ui->spacingLine->clear();
        depths.clear();
        heights.clear();
        rows.clear();
        lats.clear();
        lons.clear();
        spacing.clear();

    }
}

void MainWindow::loadImage(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        img = QImage(fileName);
        imgOut = img.scaled(iwidth, img.height(), Qt::IgnoreAspectRatio);
        ui->label->show();
        ui->scrollArea->show();
        ui->label->setPixmap(QPixmap::fromImage(imgOut));
        ui->scrollArea->setBackgroundRole(QPalette::Dark);
        ui->scrollArea->setWidget(ui->label);
    }
}

void MainWindow::loadData()
{
    if(IdxFile != NULL)
        fclose(IdxFile);
    IdxFile = fopen(idxName.toLocal8Bit().data(), "rb");
    fseek(IdxFile,20,0);
    fread(buffer, 4, 1, IdxFile);
    hp = *((int *)buffer);
    fseek(IdxFile, hp, 0);
    fread(&IdxHeader, sizeof(idx_header), 1, IdxFile);
    fseek(IdxFile, IdxHeader.data_pointer, 0);
    struct tm *t;
    char temp_time[100];
    t = localtime(&IdxHeader.time);
    strftime(temp_time, 100, "%d.%m.%Y %H:%M:%S", t);
    ui->timeCreate->setText(temp_time);
    rowsCount = IdxHeader.total_str;
    ui->tableWidget->setRowCount(rowsCount);
    depths.clear();
    heights.clear();
    rows.clear();
    lats.clear();
    lons.clear();
    spacing.clear();
    QVector<time_t> times;
    QVector<float> speeds;
    for(int i = 0; fread(&IdxData, sizeof(idx_data), 1, IdxFile)==1; i++)
    {
        t = localtime(&IdxData.time);
        strftime(temp_time, 100, "%d.%m.%Y %H:%M:%S", t);
        ui->tableWidget->setItem(i,0, new QTableWidgetItem(QString("%1").arg(temp_time)));
        ui->tableWidget->setItem(i,1, new QTableWidgetItem(QString("%1").arg(IdxData.tauzi)));
        ui->tableWidget->setItem(i,2, new QTableWidgetItem(QString("%1").arg(IdxData.heading,0,'f',2)));
        ui->tableWidget->setItem(i,3, new QTableWidgetItem(QString("%1").arg(IdxData.roll,0,'f',5)));
        ui->tableWidget->setItem(i,4, new QTableWidgetItem(QString("%1").arg(IdxData.pitch,0,'f',5)));
        ui->tableWidget->setItem(i,5, new QTableWidgetItem(QString("%1").arg(IdxData.speed,0,'f',5)));
        ui->tableWidget->setItem(i,6, new QTableWidgetItem(QString("%1").arg(IdxData.vspeed,0,'f',5)));
        double grad, minutes, seconds;
        grad = IdxData.lat*180.0/M_PI;
        minutes = modf(grad, &grad);
        minutes *= 60.0;
        seconds = modf(minutes, &minutes);
        seconds *= 60.0;
        char buff[100];
        sprintf(buff, "%.0f°%.0f'%.2f''", grad, minutes, seconds);
        ui->tableWidget->setItem(i,7, new QTableWidgetItem(QString("%1").arg(buff)));
        grad = IdxData.lon*180.0/M_PI;
        minutes = modf(grad, &grad);
        minutes *= 60.0;
        seconds = modf(minutes, &minutes);
        seconds *= 60.0;
        sprintf(buff, "%.0f°%.0f'%.2f''", grad, minutes, seconds);
        ui->tableWidget->setItem(i,8, new QTableWidgetItem(QString("%1").arg(buff)));
        ui->tableWidget->setItem(i,9, new QTableWidgetItem(QString("%1").arg(IdxData.alt,0,'f',3)));
        ui->tableWidget->setItem(i,10, new QTableWidgetItem(QString("%1").arg(IdxData.depth,0,'f',3)));
        depths.append(-IdxData.depth);
        heights.append(depths[i] - IdxData.alt);
        lats.append(IdxData.lat*180.0/M_PI);
        lons.append(IdxData.lon*180.0/M_PI);
        rows.append(i);
        times.append(IdxData.time);
        speeds.append(IdxData.speed);
        if(i == 0){
            spacing.append(0);
        }
        else{
            spacing.append(spacing[i-1] + speeds[i-1]*(times[i] - times[i-1]));
        }
    }
    drawRelief(depths, heights, rows, rowsCount);
    drawTrek(lons, lats);
    ui->tableWidget->selectRow(0);
    reliefTracer->setGraphKey(0);
    ui->reliefPlot->replot();
    trekTracer->setGraphKey(lons[0]);
    ui->trekPlot->replot();
    ui->spacingLine->setText(QString("%1").arg(spacing[0],0,'f',4));
    fclose(IdxFile);
}

void MainWindow::drawRelief(QVector<double> y1, QVector<double> y2, QVector<double> x, int N)
{
    double min = *std::min_element(y2.begin(),y2.end()) - 2;
    ui->reliefPlot->clearGraphs();
    ui->reliefPlot->addGraph();
    ui->reliefPlot->graph(0)->setPen(QPen(Qt::gray));
    ui->reliefPlot->graph(0)->setData(x, y2);
    ui->reliefPlot->graph(0)->setBrush(QBrush(QColor(0, 190, 210)));
    ui->reliefPlot->addGraph();
    ui->reliefPlot->graph(1)->setPen(QPen(Qt::red));
    ui->reliefPlot->graph(1)->setData(x, y1);
    reliefTracer->setGraph(ui->reliefPlot->graph(1));
    ui->reliefPlot->addLayer("abovemain", ui->reliefPlot->layer("main"), QCustomPlot::limAbove);
    reliefTracer->setLayer("abovemain");
    ui->reliefPlot->axisRect()->setBackground(Qt::gray);
    ui->reliefPlot->xAxis->setRange(0, N);
    ui->reliefPlot->yAxis->setRange(min, 0);
    ui->reliefPlot->yAxis->setLabel("Метры (м)");
    ui->reliefPlot->replot();
}

void MainWindow::drawTrek(QVector<double> x, QVector<double> y)
{
    double minX = *std::min_element(x.begin(),x.end());
    double maxX = *std::max_element(x.begin(),x.end());
    double minY = *std::min_element(y.begin(),y.end());
    double maxY = *std::max_element(y.begin(),y.end());
    double dif;
    if (maxY - minY > maxX - minX) {
        dif = maxY - minY;
    }
    else {
        dif = maxX - minX;
    }
    QSharedPointer<QCPAxisTickerText> textTicker1(new QCPAxisTickerText);
    double grad, minutes, seconds;
    char buff[100];
    for(double i = (minX+maxX)/2.0 - dif*0.5; i <= (minX+maxX)/2.0 + dif*0.5; i+=dif/3.0){
        grad = floor(i*6000)/6000;
        minutes = modf(grad, &grad)*60.0;
        seconds = modf(minutes, &minutes)*60.0;
        sprintf(buff, "%.0f°%.0f'%.2f''", grad, minutes, seconds);
        textTicker1->addTick(floor(i*6000)/6000, buff);
    }
    ui->trekPlot->xAxis->setTicker(textTicker1);
    QSharedPointer<QCPAxisTickerText> textTicker2(new QCPAxisTickerText);
    for(double i = (minY+maxY)/2.0 - dif*0.5; i <= (minY+maxY)/2.0 + dif*0.5; i+=dif/3.0){
        grad = floor(i*6000)/6000;
        minutes = modf(grad, &grad)*60.0;
        seconds = modf(minutes, &minutes)*60.0;
        sprintf(buff, "%.0f°%.0f'%.2f''", grad, minutes, seconds);
        textTicker2->addTick(floor(i*6000)/6000, buff);
    }
    ui->trekPlot->yAxis->setTicker(textTicker2);
    ui->trekPlot->clearGraphs();
    ui->trekPlot->addGraph();
    ui->trekPlot->graph()->setPen(QPen(QColor(220, 140, 10)));
    ui->trekPlot->graph()->setData(x, y);
    ui->trekPlot->graph()->setLineStyle(QCPGraph::lsNone);
    ui->trekPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 2));
    trekTracer->setGraph(ui->trekPlot->graph());
    ui->trekPlot->xAxis->setRange((minX+maxX)/2.0 - dif*0.6, (minX+maxX)/2.0 + dif*0.6);
    ui->trekPlot->yAxis->setRange((minY+maxY)/2.0 - dif*0.6, (minY+maxY)/2.0 + dif*0.6);
    ui->trekPlot->xAxis->setLabel("Градусы, минуты, секунды");
    ui->trekPlot->yAxis->setLabel("Градусы, минуты, секунды");
    ui->trekPlot->replot();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        mouseEvent(event);
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(QApplication::mouseButtons()) {
        mouseEvent(event);
    }
}

void MainWindow::mouseEvent(QMouseEvent *event)
{
    Cursor = ui->scrollArea->mapFromParent(event->pos());
    QPixmap px = QPixmap::fromImage(imgOut);
    QPainter paint (&px);
    paint.setPen(Qt::red);
    try {
        if (IdxFile == NULL or gboName == "" or idxName == ""){
            throw -1;
        }
        if (0 < Cursor.x() && Cursor.x() < ui->scrollArea->width()-20 && ui->menubar->sizeHint().height() < Cursor.y() && Cursor.y()< ui->scrollArea->height() + ui->menubar->sizeHint().height() - 1){
            ui->tableWidget->selectRow(-(ui->label->y() - Cursor.y()+ui->menubar->sizeHint().height()+1));
            ui->tableWidget->setFocus();
            reliefTracer->setGraphKey(-(ui->label->y() - Cursor.y()+ui->menubar->sizeHint().height()+1));
            ui->reliefPlot->replot();
            trekTracer->setGraphKey(lons[-(ui->label->y() - Cursor.y()+ui->menubar->sizeHint().height()+1)]);
            ui->trekPlot->replot();
            paint.drawLine(0,-(ui->label->y() - Cursor.y()+ui->menubar->sizeHint().height()+1), iwidth,-(ui->label->y() - Cursor.y()+ui->menubar->sizeHint().height()+1));
            paint.end();
            ui->label->setPixmap(px);
            drawHist(-(ui->label->y() - Cursor.y()+21));
            ui->spacingLine->setText(QString("%1").arg(spacing[-(ui->label->y() - Cursor.y()+21)],0,'f',4));
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

void MainWindow::reliefEvent(QMouseEvent *event)
{
    if (QApplication::mouseButtons() == Qt::LeftButton) {
        double coordX = ui->reliefPlot->xAxis->pixelToCoord(event->pos().x());
        QPixmap px = QPixmap::fromImage(imgOut);
        QPainter paint (&px);
        if(coordX >= 0 && coordX < IdxHeader.total_str && spacing.length() > 0){
            reliefTracer->setGraphKey(coordX);
            ui->reliefPlot->replot();
            trekTracer->setGraphKey(lons[coordX]);
            ui->trekPlot->replot();
            ui->tableWidget->selectRow(((int) coordX));
            ui->tableWidget->setFocus();
            paint.setPen(Qt::red);
            paint.drawLine(0,((int) coordX), iwidth,((int) coordX));
            paint.end();
            ui->scrollArea->verticalScrollBar()->setValue(((int) coordX)-100);
            ui->label->setPixmap(px);
            drawHist(coordX);
            ui->spacingLine->setText(QString("%1").arg(spacing[coordX],0,'f',4));
        }
    }
}

void MainWindow::on_tableWidget_cellClicked(int row)
{
    if(spacing.length()>0){
        QPixmap px = QPixmap::fromImage(imgOut);
        QPainter paint (&px);
        reliefTracer->setGraphKey(row);
        ui->reliefPlot->replot();
        trekTracer->setGraphKey(lons[row]);
        ui->trekPlot->replot();
        ui->tableWidget->selectRow(row);
        paint.setPen(Qt::red);
        paint.drawLine(0, row, iwidth, row);
        paint.end();
        ui->scrollArea->verticalScrollBar()->setValue(row-100);
        ui->label->setPixmap(px);
        drawHist(row);
        ui->spacingLine->setText(QString("%1").arg(spacing[row],0,'f',4));
    }
}

void MainWindow::drawHist(int line){
    QVector<double> X(img.width()), Y(img.width());
    for(int i = 0; i < img.width(); i++){
        X[i] = i;
        Y[i] = img.pixelColor(i,line).value();
    }
    ui->histoPlot->clearGraphs();
    ui->histoPlot->addGraph();
    ui->histoPlot->graph()->setPen(QPen(Qt::blue));
    ui->histoPlot->graph()->setData(X, Y);
    ui->histoPlot->xAxis->setRange(0, img.width());
    ui->histoPlot->replot();
}

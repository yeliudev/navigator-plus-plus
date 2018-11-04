﻿#include <QFileDialog>
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
using namespace std;

MainWindow::MainWindow() : ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    elidfont = new QFontMetrics(ui->pathLabel->font());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *size)
{
    // 启用省略模式输出路径
    ui->pathLabel->setText(elidfont->elidedText(path, Qt::ElideMiddle, ui->pathLabel->width()));
}

void MainWindow::on_openFileButton_clicked()
{
    // 打开模式对话框，选择文件
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", filePath, "*.e00");

    // 保存上次打开的目录
    filePath = fileName.section('/', -2);

    // 文件格式校验
    if (fileName.section('.', -1) == "e00")
    {
        // 建立信号和槽的连接
        map = new QGeoMap(this);
        connect(map, SIGNAL(pathUpdated(QString)), this, SLOT(on_pathUpdated(QString)));

        // 读取文件并解码
        if (map->loadMap(fileName.toStdString()))
        {
            // 更新标题栏文字
            this->setWindowTitle(fileName.section('/', -1) + " - Navigator++");

            // 清除路径
            path.clear();
            ui->pathLabel->setText("");

            // 渲染图形
            ui->mapWidget->setMap(map);
            ui->mapWidget->resetOffset();
            ui->mapWidget->update();
        }
        else
        {
            // 释放无用内存
            delete map;
            map = nullptr;
        }
    }
    else if (fileName.length())
    {
        QMessageBox::critical(this, "Error", "Please choose a \".e00\" file", QMessageBox::Ok);
    }
}

void MainWindow::on_analyzeButton_clicked()
{
    // 检查是否已打开地图文件
    if (map != nullptr)
    {
        Dialog dialog(map);
        dialog.exec();
    }
    else
    {
        QMessageBox::information(this, "Notice", "Please open a map first", QMessageBox::Ok);
    }
}

void MainWindow::on_pathUpdated(QString path)
{
    // 更新要输出的路径字符串
    this->path = path;

    // 启用省略模式输出路径字符串
    ui->pathLabel->setText(elidfont->elidedText(path, Qt::ElideMiddle, ui->pathLabel->width()));

    // 绘制路径
    ui->mapWidget->update();
}

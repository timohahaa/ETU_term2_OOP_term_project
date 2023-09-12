
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_play_pushButton_2_clicked()
{
    QCoreApplication::quit();
}


void MainWindow::on_play_pushButton_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_pushButton_2_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(2);
    //времянка блять!!!

    this->ui->stackedWidget->setCurrentIndex(3);
    QThread::sleep(3);
}

void MainWindow::on_pushButton_4_clicked()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

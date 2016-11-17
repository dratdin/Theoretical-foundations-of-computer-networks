#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

//Handle creating of GUI for mr.Glitsevich
void MainWindow::configurateGUI()
{
    this->setWindowTitle("Com ports");
    this->setFixedSize(400, 350);
    QLabel *inputLabel = new QLabel(this);
    inputLabel->setText("Input:");
    inputLabel->setGeometry(30,40,40,40);
    inputField = new QTextEdit(this);
    inputField->setGeometry(80,30,230,60);
    QLabel *outputLabel = new QLabel(this);
    outputLabel->setText("Output:");
    outputLabel->setGeometry(20,110,50,40);
    outputField = new QTextEdit(this);
    outputField->setGeometry(80,100,230,60);
    QLabel *debugLabel = new QLabel(this);
    debugLabel->setText("Debug:");
    debugLabel->setGeometry(25,180,50,40);
    debugField = new QTextEdit(this);
    debugField->setGeometry(80,180,230,140);
    sendButton = new QPushButton(this);
    sendButton->setGeometry(330,80,45,30);
    sendButton->setText("Send");
    connect(sendButton,SIGNAL(clicked(bool)),this,SLOT(writeToCOM2()));
    connect(&com1, SIGNAL(readyRead()), this, SLOT(readFromCOM1()));
}

void MainWindow::connectPorts(QString nameCOM1, QString nameCOM2)
{
    com1.setPortName(nameCOM1);
    initializePort(com1);
    com2.setPortName(nameCOM2);
    initializePort(com2);
}

void MainWindow::initializePort(QSerialPort &port)
{
    port.setBaudRate(QSerialPort::Baud9600);
    port.setStopBits(QSerialPort::OneStop);
    port.setParity(QSerialPort::NoParity);
    port.setDataBits(QSerialPort::Data8);
    port.setFlowControl(QSerialPort::NoFlowControl);
    port.open(QIODevice::ReadWrite);
}

void MainWindow::writeToCOM2()
{
    outputField->clear();
    com2.write(inputField->toPlainText().toLocal8Bit());
    inputField->clear();
}

void MainWindow::readFromCOM1()
{
    outputField->insertPlainText(com1.readAll());
}

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

/* Package structure:
 * |Flag(1)|DA(1)|SA(1)|Data(7)|FCS(1)|
 * (1) - size in Bytes */
#define FLAG 126
#define DA 123
#define SA 124
#define FCS 39
#define PACKAGE_SIZE 11
#define DATA_INDEX_START 3

void MainWindow::writeToCOM2()
{
    debugField->clear();
    outputField->clear();
    QString text = inputField->toPlainText();
    for(int i = 0; i < text.length(); i+=7) {
        QString data = text.mid(i,7);
        com2.write(pack(FLAG,DA,SA,data,FCS));
    }
    inputField->clear();
}

void MainWindow::readFromCOM1()
{
    char buffer[1];
    com1.read(buffer,1);
    while(buffer[0] == FLAG) {
        com1.read(buffer,1);//DA
        com1.read(buffer,1);//SA
        QByteArray data;
        while(com1.read(buffer,1) && buffer[0] != FLAG) {
            data.append(buffer[0]);
        }
        data = byteStuffingDecode(data);
        data = unpack(data);
        outputField->insertPlainText(data);
    }
}

QByteArray MainWindow::pack(QChar flag, QChar da, QChar sa, QString data, QChar fcs)
{
    QByteArray package;
    package.append(flag);
    package.append(da);
    package.append(sa);
    package.append(data);
    while(package.length() < PACKAGE_SIZE - 1) {
        package.append(31);
    }
    package.append(fcs);
    if(package.length() != PACKAGE_SIZE) {
        debugField->append("Wrap up error! Package size:" + QString::number(package.length()));
        //+1 для нуль терминатора в конце
    }
    package = byteStuffingCode(package);
    debugField->append(package);
    return package;
}

QByteArray MainWindow::unpack(QByteArray package)
{
    package = byteStuffingDecode(package);
    package.remove(package.length()-1,1);
    debugField->append(QString::number(package.length()));
    return package;
}

QByteArray MainWindow::byteStuffingCode(QByteArray package)
{
    for(int i = DATA_INDEX_START ; i < package.length()-1 ; i++) {
        if((char)package[i] == 126) {
            package[i] = 125; //change 7E to 7D
            package.insert(i+1,94); //insert 5E after i
        } else if((char)package[i] == 125) {
            package.insert(i+1,93); //insert 5D after i
        }
    }
    return package;
}

QByteArray MainWindow::byteStuffingDecode(QByteArray package)
{
    for(int i = 0 ; i < package.length()-1 ; i++) {
        if((char)package[i] == 125) {
            if((char)package[i+1] == 94) {
                package[i] = 126;
                package.remove(i+1,1);
            } else if((char)package[i+1] == 93) {
                package.remove(i+1,1);
            }
        }
    }
    return package;
}

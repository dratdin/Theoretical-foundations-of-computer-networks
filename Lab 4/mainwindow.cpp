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

/* TRANSMITTER LOGIC */
#define MAX_COUNT_TRY 10
#define JAM_SIGNAl 126
#define CHANNEL_WAIT_MS 500
#define COLLISION_WINDOW_WAIT_MS 200
QByteArray MainWindow::CSMACD_Transmitter(QByteArray data)
{
    QByteArray buffer;
    for(int i = 0 ; i < data.length() ; i++) {
        int tryCounter = 0;
        while(true) {
            while(!isChannelFree()) {
                Sleeper::msleep(CHANNEL_WAIT_MS);
            }
            buffer.append((QChar)data[i]);
            Sleeper::msleep(COLLISION_WINDOW_WAIT_MS);
            if(isCollision()) {
                buffer.append(JAM_SIGNAl);
                tryCounter++;
                if(tryCounter >=  MAX_COUNT_TRY) {
                    debugField->insertPlainText("_Transfer ERROR!!!_");
                    return buffer;
                }
                Sleeper::msleep(calculateDelayTime(tryCounter));
            } else
                break;
        }
    }
    return buffer;
}

bool MainWindow::isEvenSecond()
{
    time_t t = time(NULL);
    tm* aTm = localtime(&t);
    return (aTm->tm_sec % 2 != 0);
}

bool MainWindow::isChannelFree() { return !isEvenSecond(); }
bool MainWindow::isCollision() { return isEvenSecond(); }

int MainWindow::calculateDelayTime(int attempt)
{
    int slotTime = 10;
    int k = qMin(attempt, 10);
    int delayTime = (int)(rand() % (int)qPow(2,k)) * slotTime;
    return delayTime;
}

/* RECEIVER LOGIC */
QByteArray MainWindow::CSMACD_Receiver(QByteArray data)
{
    QByteArray buffer;
    for(int i = 0; i < data.length()-1 ; ) {
        if((QChar)data[i+1] != JAM_SIGNAl) {
            buffer.append((QChar)data[i]);
            i++;
        } else {
            debugField->append((QChar)data[i]);
            debugField->append((QChar)'X');
            i+=2;
        }
    }
    buffer.append((QChar)data[data.length()-1]);
    return buffer;
}

void MainWindow::writeToCOM2()
{
    outputField->clear();
    QByteArray inputData = inputField->toPlainText().toLocal8Bit();
    com2.write(CSMACD_Transmitter(inputData));
    inputField->clear();
}

void MainWindow::readFromCOM1()
{
    QByteArray receivedData = com1.readAll();
    outputField->insertPlainText(CSMACD_Receiver(receivedData));
}

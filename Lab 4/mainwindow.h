#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QtCore/QCoreApplication>
#include <QtSerialPort/QtSerialPort>
#include <time.h>
#include <QThread>
#include <QtGlobal>
#include <QtCore/qmath.h>

namespace Ui {
 class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTextEdit *inputField;
    QTextEdit *outputField;
    QTextEdit *debugField;
    QPushButton *sendButton;
    QSerialPort com1;
    QSerialPort com2;

    void connectPorts(QString nameCOM1, QString nameCOM2);
    void configurateGUI();

    class Sleeper: public QThread {
       public: static void msleep(int ms) { QThread::msleep(ms); }
    };

private:
    Ui::MainWindow *ui;
    void initializePort(QSerialPort &port);
    bool isEvenSecond();
    bool isChannelFree();
    bool isCollision();
    void collisionWindow();
    int calculateDelayTime(int attempt);

    QByteArray CSMACD_Transmitter(QByteArray data);
    QByteArray CSMACD_Receiver(QByteArray data);

public slots:
    void readFromCOM1();
    void writeToCOM2();
};

#endif // MAINWINDOW_H

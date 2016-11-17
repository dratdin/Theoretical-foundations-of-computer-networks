#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QtCore/QCoreApplication>
#include <QtSerialPort/QtSerialPort>

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

private:
    Ui::MainWindow *ui;
    void initializePort(QSerialPort &port);

public slots:
    void readFromCOM1();
    void writeToCOM2();
};

#endif // MAINWINDOW_H

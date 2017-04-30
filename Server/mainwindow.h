#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QVector>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QTcpServer>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

private slots:
    void socketNewConnection();
    void socketReadyRead();
    void socketDisconnected();

    void processStopped(int code);

private:
    static const quint16 defaultPort = 1234;

    Ui::MainWindow *ui;
    QTcpServer* server;
    QTcpSocket* socket;
    QTcpSocket* answer_socket;

    QVector<QProcess*> started_processes;

    void setup();
};

#endif // MAINWINDOW_H


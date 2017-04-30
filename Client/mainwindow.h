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
    void on_btn_clicked();

private:
    static const quint16 defaultPort = 1234;

    Ui::MainWindow *ui;
    QTcpServer* server;
    QTcpSocket* socket;
    QTcpSocket* answer_socket;

    QVector<QString> started_programs;

    void setup();
    void addProgramLabel(QString newProg);
    void deleteName(int pos);
};

#endif // MAINWINDOW_H

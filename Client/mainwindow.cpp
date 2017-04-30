#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addProgramLabel(QString newProg)
{
    started_programs.push_back(newProg);

    ui->programslbl->setText(ui->programslbl->text()+  "\n" + newProg);
}

void MainWindow::on_btn_clicked()
{
    QString path = ui->progName->text();

    if (path.isEmpty()) return;

    qDebug() << "Sending: " + path;

    socket = new QTcpSocket(this);
    QHostAddress address("192.168.0.100");
    //socket->connectToHost(QHostAddress::LocalHost, 1234);
    socket->connectToHost(address, 1234);
    socket->waitForConnected(3000);

    QString nameStr = path.split("\\").last();
    addProgramLabel(nameStr);

    QByteArray path1 = path.toUtf8();

    socket->write(path1);
    socket->flush();
    socket->waitForBytesWritten(3000);
    socket->close();
}

void MainWindow::setup()
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(socketNewConnection()));

    if (!server->listen(QHostAddress::Any, 5678)){
        qDebug() << "TCP server error";
    }
    else {
        qDebug() << "TCP server started";
    }
}

void MainWindow::socketNewConnection()
{
    qDebug() << "new connection";
    answer_socket = new QTcpSocket(this);
    answer_socket = server->nextPendingConnection();
    connect(answer_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(answer_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

void MainWindow::socketReadyRead()
{
    QByteArray data = answer_socket->readAll();
    qDebug() << "data received:" << data;

    QDataStream ds(data);
    int pos;
    ds >> pos;

    deleteName(pos);

}

void MainWindow::socketDisconnected()
{
    disconnect(answer_socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    disconnect(answer_socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    qDebug() << "disconnected";

    answer_socket->deleteLater();
}

void MainWindow::deleteName(int pos)
{
    auto iter = started_programs.begin();
    for(int i = 0; i < pos; i++)
        iter++;
    started_programs.erase(iter);

    QString str = "";
    for(auto name : started_programs)
        str += "\n" + name;
    ui->programslbl->setText(str);
}


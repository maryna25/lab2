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

void MainWindow::setup()
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(socketNewConnection()));

    if (!server->listen(QHostAddress::Any, defaultPort)){
        qDebug() << "TCP server error";
    }
    else {
        qDebug() << "TCP server started";
    }
}

void MainWindow::socketNewConnection()
{
    qDebug() << "new connection";
    socket = new QTcpSocket(this);
    socket = server->nextPendingConnection();
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
}

void MainWindow::socketReadyRead()
{
    QByteArray data = socket->readAll();
    qDebug() << "data received:" << data;

    QProcess *p = new QProcess(this);
    //p->start("d:\KMPlayer\KMPlayer.exe");
    p->start(data);
    if(p->pid() == 0)
        sendPos(started_processes.size());
    else
    {
        connect(p,SIGNAL(finished(int)), this, SLOT(processStopped(int)));
        started_processes.push_back(p);
    }

}

void MainWindow::socketDisconnected()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    qDebug() << "disconnected";

    socket->deleteLater();
}

void MainWindow::processStopped(int code)
{
    int pos = 0;
    for(auto iter = started_processes.begin(); iter != started_processes.end(); iter++)
    {
        if((*iter)->pid() == 0)
        {
            started_processes.erase(iter);
            break;
        }
        pos++;
    }

    qDebug() << "Sending: " << pos;

    sendPos(pos);

}

void MainWindow::on_btn_clicked()
{
    client_address.setAddress(ui->client_inp->text());
    defaultPort = ui->port_inp->text().toInt();
    ui->btn->setDisabled(true);
}

void MainWindow::sendPos(int pos)
{
    answer_socket = new QTcpSocket(this);
    //QHostAddress address("192.168.0.102");
    answer_socket->connectToHost(client_address, defaultPort);
    answer_socket->waitForConnected(3000);

    QByteArray p;
    QDataStream stream(&p, QIODevice::ReadWrite);

    stream << pos;

    answer_socket->write(p);
    answer_socket->flush();
    answer_socket->waitForBytesWritten(3000);
    answer_socket->close();
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progName->setDisabled(true);
    ui->btn->setDisabled(true);

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
    socket->connectToHost(server_address, defaultPort);
    socket->waitForConnected(3000);

    QByteArray path1 = path.toUtf8();

    socket->write(path1);
    socket->flush();
    if(!socket->waitForBytesWritten(3000))
    {
        QString nameStr = path.split("\\").last();
        addProgramLabel(nameStr);
    }
    else
        qDebug() << "Can't send!";

    socket->close();
}

void MainWindow::setup()
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(socketNewConnection()));

    if (!server->listen(QHostAddress::Any, defaultPort)){ //**
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


void MainWindow::on_address_btn_clicked()
{
    server_address.setAddress(ui->address_inp->text());
    defaultPort = ui->port_inp->text().toInt();

    ui->progName->setDisabled(false);
    ui->btn->setDisabled(false);
    ui->address_btn->setDisabled(true);

    setup();
}

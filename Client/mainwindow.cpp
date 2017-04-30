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
    QHostAddress address("192.168.0.102");
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

    if (!server->listen(QHostAddress::Any, 1234)){
        qDebug() << "TCP server error";
    }
    else {
        qDebug() << "TCP server started";
    }
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


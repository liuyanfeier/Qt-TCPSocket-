#include "server.h"
#include "ui_server.h"

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);

    connect (ui->serverSendpushButton, SIGNAL(clicked()), this, SLOT(serverSendMessage()));
    ui->serverSendpushButton->setEnabled (false);
    ui->serverSendpushButton->setEnabled (true);
    ui->stoppushButton->setEnabled (false);
}

Server::~Server()
{
    delete ui;
}


//服务器端读取信息
void Server::serverReadMessage()
{
    QDataStream in(clientConnection);
    in.setVersion (QDataStream::Qt_4_7);
    if (blockSize == 0) {
        // 判断接收的数据是否有两字节，也就是文件的大小信息
        // 如果有则保存到 blockSize 变量中，没有则返回，继续接收数据
        if(clientConnection->bytesAvailable () < (int)sizeof(quint16))
            return ;
        in >> blockSize;
    }
    if(clientConnection->bytesAvailable () < blockSize)// 如果没有得到全部的数据，则返回，继续接收数据
        return;
    in >> message;
    if (message.contains ("clientStop")) { //如果收到是客户端断开连接的信息
        clientConnection->close ();
        ui->serverSendpushButton->setEnabled (false);
        ui->statuslabel->setText (tr("客户端断开连接"));
        blockSize = 0;
        return;
    }
    ui->servertextBrowser->insertPlainText(tr("reveived message: %1 \n").arg(message));
    blockSize = 0;
}

//服务器端发送信息
void Server::serverSendMessage ()
{
    if(!clientConnection) //判断有没有实例化
        if (!(clientConnection = tcpServer->nextPendingConnection ())) { //没有客户端连接
            return;
        }
    serverMessage = ui->serverMessagelineEdit->text ();
    if (serverMessage.isEmpty()) {
        QMessageBox::warning (this, tr("Warnning"), tr("请输入发送数据"));
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_4_7);
    out << (quint16) 0;
    out << serverMessage;
    out.device ()->seek (0);
    out << (quint16) (block.size () - sizeof(quint16));
    connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayErrorS(QAbstractSocket::SocketError)));
    clientConnection->write (block);
    if(serverMessage.contains ("serverStop"))
        return;
    ui->servertextBrowser->insertPlainText (tr("send message: %1 \n").arg (serverMessage));

    ui->serverMessagelineEdit->clear();
}

void Server::getLocalIP()
{
    IPlist = QNetworkInterface::allAddresses();

    foreach (QHostAddress IP, IPlist) {
        ui->serverIPcomboBox->addItem (IP.toString());
    }
}

//侦听按钮槽函数
void Server::on_listenpushButton_clicked()
{
    serverPort = ui->serverPortlineEdit->text();
    serverIP = ui->serverIPcomboBox->currentText();

    if (serverPort.isEmpty()) {
        QMessageBox::warning (this, tr("Warnning"), tr("端口号不能为空"));
        return;
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, serverPort.toInt())) {
        QMessageBox::warning (this, tr("Warnning"), tcpServer->errorString());
        close ();
    }

    connect (tcpServer, SIGNAL(newConnection()), this, SLOT(updateStatus()));
    connect (tcpServer, SIGNAL(newConnection()), this, SLOT(screateServerSocket()));  //有新的连接到来，则开始创建套接字

    ui->statuslabel->setText(tr("开始监听"));
    ui->listenpushButton->setEnabled(false);
    ui->stoppushButton->setEnabled(true);

    //QTimer *time = new QTimer(this); //更新状态标签
    //connect (time, SIGNAL(timeout()), this, SLOT(updateStatus()));
    //time->start ();
}

//需要自己添加的函数
void Server::updateStatus()
{
    //ui->statuslabel->setText((tr("客户端已连接")));
    //ui->serverSendpushButton->setEnabled (true);
}

//服务器端创建套接字
void Server::screateServerSocket()
{
    ui->statuslabel->setText(tr("客户端已连接"));
    ui->serverSendpushButton->setEnabled (true);
    clientConnection = tcpServer->nextPendingConnection();

    blockSize = 0;//初始化

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    connect(clientConnection, SIGNAL(readyRead()), this, SLOT(serverReadMessage()));
}

 //服务器端错误提示
void Server::displayErrorS(QAbstractSocket::SocketError)
{
  //  QMessageBox::warning (this, tr("Warnning"), clientConnection->errorString ());
    ui->servertextBrowser->insertPlainText(clientConnection->errorString());
    connect (clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
    clientConnection->disconnectFromHost();
    ui->statuslabel->setText(tr("断开连接"));
}


//清空按钮槽函数
void Server::on_sCleanpushButton_clicked()
{
    ui->servertextBrowser->setText("");
}

//停止按钮槽函数
void Server::on_stoppushButton_clicked()
{
    ui->serverMessagelineEdit->setText(tr("serverStop")); //发送服务端停止监听信息
    serverSendMessage();
    ui->serverMessagelineEdit->setText(tr(""));

    tcpServer->close();

    ui->statuslabel->setText(tr("停止监听"));
    ui->listenpushButton->setEnabled(true);
    ui->stoppushButton->setEnabled(false);
    ui->serverSendpushButton->setEnabled(false);
}





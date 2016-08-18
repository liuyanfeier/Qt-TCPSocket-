#include "client.h"
#include "ui_client.h"

#include <QMessageBox>
#include <QString>

client::client(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::client)
{
    ui->setupUi(this);

    connect(ui->clientSendpushButton, SIGNAL(clicked()), this, SLOT(clientSendMessage()));
//    connect(ui->clientMessagelineEdit, &QLineEdit::returnPressed, this, &client::clientSendMessage);

    ui->clientSendpushButton->setEnabled(false);
    ui->disconnectpushButton->setEnabled(false);
}

client::~client()
{
    delete ui;
}

//客户端发送信息
void client::clientSendMessage()
{
    QTime time = QTime::currentTime();
    QString now = time.toString("hh:mm:ss");

    clientMessage = ui->clientMessagelineEdit->text();
    if(clientMessage.isEmpty())
    {
        QMessageBox::warning (this, tr("Warnning"), tr("请输入发送数据"));
        return;
    }

    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion (QDataStream::Qt_4_7);
    out << (quint16) 0;
    out << clientMessage;
    out.device ()->seek (0);
    out << (quint16)(block.size() - sizeof(quint16));
    tcpClient->write(block);
    if(clientMessage.contains("clientStop"))
        return;

    qDebug() << clientMessage;

    QRegExp rr1("/clear");
    QRegExp rr2("/close");
    QRegExp rr3("/quit");
    QRegExp rr4("/list");
    QRegExp rr5("/join");

    if (rr1.exactMatch(clientMessage)) {
        ui->messagetextBrowser->clear();
    } else if (rr2.exactMatch(clientMessage)) {
        this->close();
    } else if (rr3.exactMatch(clientMessage)) {
        this->close();
    } else if (rr4.exactMatch(clientMessage)) {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg((QString)"LIST"));
    } else if (rr5.exactMatch(clientMessage)) {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg((QString)"JOIN"));
    } else {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg(now + " : " + clientMessage));
    }

    //保持编辑器在光标最后一行
    QTextCursor cursor = ui->messagetextBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->messagetextBrowser->setTextCursor(cursor);

    ui->clientMessagelineEdit->clear();
}

void client::on_cCleanpushButton_clicked()
{
    ui->messagetextBrowser->setText("");
}

//客户端连接按钮槽函数
void client::on_connectpushButton_clicked()
{
    tcpClient = new QTcpSocket(this);
    connect (tcpClient, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect (tcpClient, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect (tcpClient, SIGNAL(connected()), this, SLOT(updateClientStatusConnect())); //更新状态
    connect (tcpClient, SIGNAL(disconnected()), this, SLOT(updateClientStatusDisconnect()));
    blockSize = 0;//初始化
    tcpClient->abort();
    serverIP = ui->clientIPlineEdit->text();
    clientPort = ui->clientPortlineEdit->text();
    if(serverIP.isEmpty() || clientPort.isEmpty())
    {
        QMessageBox::warning(this, tr("Warnning"), tr("服务器IP或端口号不能为空"));
        return;
    }
    tcpClient->connectToHost (serverIP, clientPort.toInt()); //连接到主机
}

void client::on_disconnectpushButton_clicked()
{
    ui->clientMessagelineEdit->setText(tr("clientStop"));
    clientSendMessage();
    ui->clientMessagelineEdit->setText(tr(""));
    tcpClient->close();

}

//客户端错误提示
void client::displayError(QAbstractSocket::SocketError)
{
      QMessageBox::warning(this, tr("Warnning"), tcpClient->errorString());
      tcpClient->close();
      ui->connectpushButton->setEnabled(true);
      ui->disconnectpushButton->setEnabled(false);
      ui->clientSendpushButton->setEnabled(false);
}

void client::updateClientStatusConnect()
{
    ui->cStatuslabel->setText(tr("已连接"));
    ui->connectpushButton->setEnabled(false);
    ui->disconnectpushButton->setEnabled(true);
    ui->clientSendpushButton->setEnabled(true);
}

void client::updateClientStatusDisconnect()
{
    ui->cStatuslabel->setText(tr("已断开"));
    tcpClient->close();
    ui->connectpushButton->setEnabled(true);
    ui->disconnectpushButton->setEnabled(false);
    ui->clientSendpushButton->setEnabled(false);
}



//需要自己添加的函数
void client::updateStatus()
{
    //暂时为空
}

//客户端读取信息
void client::readMessage()
{
    QTime time = QTime::currentTime();
    QString now = time.toString("hh:mm:ss");

    QDataStream in(tcpClient);
    in.setVersion(QDataStream::Qt_4_7);
    if(blockSize == 0)
    {
        // 判断接收的数据是否有两字节，也就是文件的大小信息
        // 如果有则保存到 blockSize 变量中，没有则返回，继续接收数据
        if(tcpClient->bytesAvailable() < (int)sizeof(quint16))
            return ;
        in >> blockSize;
    }
    if(tcpClient->bytesAvailable() < blockSize)// 如果没有得到全部的数据，则返回，继续接收数据
        return;
    in >> message;
    if(message.contains("serverStop")) //如果收到是服务器停止监听的信息
    {
        tcpClient->close();
        ui->cStatuslabel->setText(tr("服务器断开连接"));
        blockSize = 0;
        return;
    }

    QRegExp rr1("/clear");
    QRegExp rr2("/close");
    QRegExp rr3("/quit");
    QRegExp rr4("/list");
    QRegExp rr5("/join");

    if (rr1.exactMatch(message)) {
        ui->messagetextBrowser->clear();
    } else if (rr2.exactMatch(message)) {
        this->close();
    } else if (rr3.exactMatch(message)) {
        this->close();
    } else if (rr4.exactMatch(message)) {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg((QString)"LIST"));
    } else if (rr5.exactMatch(message)) {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg((QString)"JOIN"));
    } else {
        ui->messagetextBrowser->insertPlainText(tr("send message: %1 \n").arg(now + " : " + clientMessage));
    }

    //保持编辑器在光标最后一行
    QTextCursor cursor = ui->messagetextBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->messagetextBrowser->setTextCursor(cursor);

    blockSize = 0;
}

//客户端创建套接字
void client::createServerSocket()
{
    clientConnection = tcpServer->nextPendingConnection();

    connect(clientConnection, SIGNAL(disconnected()), clientConnection, SLOT(deleteLater()));
}

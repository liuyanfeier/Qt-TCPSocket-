#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QNetworkInterface>
#include <QtNetwork>
#include <QMessageBox>

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT

public:
    explicit Server(QWidget *parent = 0);
    ~Server();

    void getLocalIP();//获取本机IP地址

private:
    Ui::Server *ui;

    QList<QHostAddress> IPlist;
    QTcpServer *tcpServer;
    QString serverPort;
    QString serverIP;
    QString serverMessage;  //服务端发出的信息
    QString message;
    quint16 blockSize;  //存放文件的大小信息
    QTcpSocket *clientConnection;

private  slots:
    void serverSendMessage();
    void on_listenpushButton_clicked();
    void updateStatus();
    void displayErrorS(QAbstractSocket::SocketError); //显示服务端错误
    void screateServerSocket();
    void on_sCleanpushButton_clicked();
    void on_stoppushButton_clicked();
    void serverReadMessage();
};

#endif // SERVER_H

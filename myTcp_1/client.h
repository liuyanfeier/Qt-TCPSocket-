#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QNetworkInterface>
#include <QtNetwork>

#include <QRegExp>
#include <QTime>

namespace Ui {
class client;
}

class client : public QDialog
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0);
    ~client();

private:
    Ui::client *ui;

    QList<QHostAddress> IPlist;
    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QString clientPort;
    QString serverIP;
    QString clientMessage;  //客户端发出的信息
    QString message;
    quint16 blockSize;  //存放文件的大小信息
    QTcpSocket *clientConnection;

private slots:
    void clientSendMessage();
    void updateStatus();
    void readMessage();  //接收数据
    void displayError(QAbstractSocket::SocketError);  //显示客户端错误
    void on_connectpushButton_clicked();
    void createServerSocket();
    void on_cCleanpushButton_clicked();
    void updateClientStatusConnect();
    void updateClientStatusDisconnect();
    void on_disconnectpushButton_clicked();
};

#endif // CLIENT_H

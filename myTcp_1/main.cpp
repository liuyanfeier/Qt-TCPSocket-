#include "client.h"
#include "server.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Server s;
    s.show();
    client c;
    c.show();

    return a.exec();
}

//
// Created by volund on 4/4/21.
//

#include <iostream>
#include "mudcore/net.h"
#include <QApplication>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    auto core = new mudcore::net::QMudServerCore((QObject*)&app);
    QHostAddress addr("0.0.0.0");
    QSslConfiguration ssl;
    core->createListener(QString("telnet"), addr, 7999, ssl, mudcore::net::MudConnectionType::Telnet);
    core->startListening();
    app.exec();
    return 0;
}
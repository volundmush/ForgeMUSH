//
// Created by volund on 4/3/21.
//

#include "mudcore/net.h"

namespace mudcore { namespace net {

    QMudConnection::QMudConnection(QObject *parent) : QObject(parent) {

    }

    QMudTelnetConnection::QMudTelnetConnection(QObject *parent, QSslSocket *socket) : QMudConnection(parent) {
        this->socket = socket;
        this->type = Telnet;
        connect(socket, &QSslSocket::readyRead, this, &QMudTelnetConnection::readyReadSocket);
    }

    void QMudTelnetConnection::startTelnetNegotiation() {
        this->writeBytes(" I exist dammit!\n");
    }

    void QMudTelnetConnection::readyReadSocket() {
        readSocket();
    }

    void QMudTelnetConnection::readSocket() {
        auto qb = socket->readAll();
    }

    qint64 QMudTelnetConnection::writeBytes(const char *data) {
       return this->socket->write(data);
    }

    QSslServer::QSslServer(QObject *parent, QSslConfiguration& config) : QTcpServer(parent), ssl_config(config) {

    }

    void QSslServer::incomingConnection(qintptr socket) {
        std::cout << "Detected connection: " << socket << std::endl;
        auto serverSocket = new QSslSocket;
        if (serverSocket->setSocketDescriptor(socket)) {
            if(ssl_config.isNull()) {
                emit connReady(serverSocket);
            } else {
                connect(serverSocket, &QSslSocket::encrypted, this, &QSslServer::ssl_ready);
                pending_ssl.insert(serverSocket);
                serverSocket->startServerEncryption();
            }
        } else {
            delete serverSocket;
        }
    }

    void QSslServer::ssl_ready() {
        // QSslSocket::encrypted doesn't give a good way to identify WHICH pending ssl connection has finished
        // encrypting itself... so we'll just iterate over the QSet.
        QSet<QSslSocket*> to_remove;
        for(auto s : pending_ssl) {
            if(s->isEncrypted()) {
                emit connReady(s);
                to_remove.insert(s);
            }
        }
        for(auto s : to_remove) {
            pending_ssl.remove(s);
        }

    }



    void QMudListener::acceptTelnetConnection(QSslSocket *socket) {
        auto conn = new QMudTelnetConnection((QObject*)this, socket);
        connect(conn, &QMudTelnetConnection::telnetReady, this, &QMudListener::telnetReady);
        conn->startTelnetNegotiation();
    }

    void QMudListener::telnetReady(QMudTelnetConnection *conn) {
        pending_telnet.remove(conn);
        emit newConnection((QMudConnection*)conn);
    }


    QMudListener::QMudListener(QMudServerCore *core, QHostAddress &address, quint16 port,
                               QSslConfiguration &config, MudConnectionType type) : QObject(core), ssl_config(config), address(address) {

        this->port = port;
        this->type = type;

        switch(type) {
            case Telnet:
                this->listener = new QSslServer((QObject*)this, ssl_config);
                connect(listener, &QSslServer::connReady, this, &QMudListener::acceptTelnetConnection);
                break;
            case WebSocket:
                // Not implemented yet!
                break;
            default:
                // this should not happen!
                break;
        }
    }

    bool QMudListener::start() {
        switch(type) {
            case Telnet:
                if(listener->isListening()) {

                } else {
                    std::cout << "Yeah started listening!" << std::endl;
                    return listener->listen(address, port);
                }
                break;
            case WebSocket:
                // not implemented yet...
                break;
        }
        return false;
    }

    bool QMudListener::stop() {
        switch(type) {
            case Telnet:
                if(listener->isListening()) {
                    listener->pauseAccepting();
                    return true;
                }
                break;
            case WebSocket:
                // not implemented yet.
                break;
        }
        return false;
    }


    void QMudServerCore::createListener(QString name, QHostAddress& interface, quint16 port, QSslConfiguration& ssl, MudConnectionType type) {

        auto l = new QMudListener(this, interface, port, ssl, type);
        l->setObjectName(name);
        connect(l, &QMudListener::newConnection, this, &QMudServerCore::acceptConnection);
        listeners.insert(l->objectName(), l);
    };

    void QMudServerCore::startListening() {
        for(auto l : listeners.values()) {
            if(!l->start()) {
                std::cout << "Something didn't listen!" << std::endl;
            }
        }
    }

    void QMudServerCore::stopListening() {
        for(auto l : listeners.values()) {
            if(!l->stop()) {
                // oops, something happened...
            }
        }
    }

    void QMudServerCore::acceptConnection(QMudConnection *conn) {
        conn->setObjectName(conn->parent()->objectName() + "_test_conn");
        std::cout << "Got a connection: " << conn->objectName().toStdString() << std::endl;
        connections.insert(conn->objectName(), conn);

        emit newConnection(conn);
    }

    QMudServerCore::QMudServerCore(QObject *parent) : QObject(parent) {

    }

    QMudServerCore::~QMudServerCore() {
        stopListening();
        for(auto l : listeners.values()) {
            delete l;
        }
        for(auto c : connections.values()) {
            delete c;
        }
    }

} }
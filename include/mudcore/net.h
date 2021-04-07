//
// Created by volund on 4/3/21.
//

#ifndef MUDCORE_NET_H
#define MUDCORE_NET_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QSslSocket>
#include <QSslConfiguration>
#include <QHash>
#include <QSet>
#include <QStack>
#include <QWebSocket>
#include <QWebSocketServer>
#include <iostream>
#include <boost/asio.hpp>

namespace mudcore { namespace net {

    enum TelnetCode : quint8 {
        NUL = 0,
        BEL = 7,
        CR = 13,
        LF = 10,
        SGA = 3,
        TELOPT_EOR = 25,
        NAWS = 31,
        LINEMODE = 34,
        EOR = 239,
        SE = 240,
        NOP = 241,
        GA = 249,
        SB = 250,
        WILL = 251,
        WONT = 252,
        DO = 253,
        DONT = 254,
        IAC = 255,

        // MNES: Mud New-Environ Standard
        MNES = 39,

        // MXP: MUD eXtension Protocol
        MXP = 91,

        // MSSP: Mud Server Status Protocol
        MSSP = 70,

        // MCCP#: Mud Client Compression Protocol
        // Not gonna support these.
        MCCP2 = 86,
        MCCP3 = 87,

        // GMCP: Generic Mud Communication Protocol
        GMCP = 201,

        // MSDP: Mud Server Data Protocol
        MSDP = 69,

        // TTYPE - Terminal Type
        TTYPE = 24
    };

    struct TelnetOptionPerspective {
        bool negotiating;
        bool enabled;
    };

    class QMudTelnetConnection;

    class QTelnetOption : public QObject {
        Q_OBJECT
    public:
        QTelnetOption(QObject *parent, TelnetCode op_code);
        void startTelnetNegotiation();
    protected:
        TelnetCode op_code;
        TelnetOptionPerspective local, remote;
    };

    enum MudConnectionType : quint8 {
        Telnet = 0,
        WebSocket = 1 // WebSocket is waiting higher Qt versions.
    };

    class QMudListener;

    class QMudConnection : public QObject {
        Q_OBJECT
    public:
        QMudConnection(QObject *parent);
        MudConnectionType type;
        virtual qint64 writeBytes(const char *data) = 0;

    signals:
        void ready(QMudConnection *rdy);
    };


    class QMudTelnetConnection : public QMudConnection {
        Q_OBJECT

    public:
        QMudTelnetConnection(QObject *parent, QSslSocket *socket);
        void startTelnetNegotiation();
        virtual qint64 writeBytes(const char *data) override;
    public slots:
        void readyReadSocket();
    signals:
        void telnetReady(QMudTelnetConnection *rdy);
    private:
        void readSocket();
        QSslSocket *socket;
        QHash<quint8, QTelnetOption*> options;
        boost::asio::streambuf read_buffer, app_buffer;
    };

    class QSslServer : public QTcpServer {
        Q_OBJECT
    public:
        QSslServer(QObject *parent, QSslConfiguration &config);
    public slots:
        void ssl_ready();
    private:
        QSslConfiguration ssl_config;
        QSet<QSslSocket*> pending_ssl;
    signals:
        void connReady(QSslSocket *socket);

    protected:
        virtual void incomingConnection(qintptr socket) override;
    };

    class QMudServerCore;

    class QMudListener : public QObject {
        Q_OBJECT
    public:
        QMudListener(QMudServerCore *core, QHostAddress &address, quint16 port, QSslConfiguration &config, MudConnectionType type);
        bool start();
        bool stop();
        QMudConnection* nextPendingConnection();
    public slots:
        void acceptTelnetConnection(QSslSocket *socket);
        void telnetReady(QMudTelnetConnection* conn);

    private:
        QMudServerCore *manager;
        QHostAddress address;
        quint16 port;
        QSslConfiguration &ssl_config;
        MudConnectionType type;
        QSet<QMudTelnetConnection*> pending_telnet;
        QSslServer *listener;
        // Will stick websocket here later. A pointer for it, anyways.
    signals:
        void newConnection(QMudConnection* conn);
    };


    class QMudServerCore : public QObject {
        Q_OBJECT
    public:
        QMudServerCore(QObject *parent);
        virtual ~QMudServerCore();
        void createListener(QString name, QHostAddress &interface, quint16 port, QSslConfiguration& ssl, MudConnectionType type);
        void startListening();
        void stopListening();
        QHash<QString, QMudListener*> listeners;
        QHash<QString, QMudConnection*> connections;
    public slots:
        void acceptConnection(QMudConnection *conn);

    signals:
        void newConnection(QMudConnection* conn);
    };

} }

#endif //MUDCORE_NET_H

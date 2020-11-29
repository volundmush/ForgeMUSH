//
// Created by volund on 11/27/20.
//

#ifndef FORGEPORTAL_PROTOCOL_H
#define FORGEPORTAL_PROTOCOL_H

#include <string>
#include <unordered_map>
#include <cstdint>
#include <list>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

namespace forgeportal::net {
    class ServerManager;
    class ConnectionManager;
    class Server;
    class Connection;
    class Protocol;
    class TCPConnection;
    class TLSConnection;

    enum ProtocolType {
        Telnet = 0,
        HTTP = 1,
        WebSocket = 2
    };

    class Protocol {
    public:
        Protocol(boost::asio::io_context& con);
        virtual void setConnection(Connection *c);
        virtual void onReceiveData(std::vector<uint8_t>& data, size_t length) = 0;
        virtual void onClose() = 0;
        virtual void onLost() = 0;
        virtual void onConnect() = 0;
    protected:
        Connection *conn = nullptr;
    };
    
    class Connection {
    public:
        Connection(Server& sr);
        virtual void receive() = 0;
        virtual void sendData() = 0;
        virtual void disconnect() = 0;
        void onReceiveData(std::vector<uint8_t> data, size_t length);
        void onClose();
        void onLost();
        virtual void onConnect() = 0;
        void setProtocol(Protocol *p);
        virtual bool isSSL() = 0;
        virtual boost::asio::ip::tcp::socket &getSocket() = 0;
        void makeProtocol();
        std::vector<uint8_t> read_buffer;
        boost::asio::ip::address getAddress();
        std::string id;
        boost::asio::io_context& context;
        Server &server;
        ServerManager& srv_manager;
        ConnectionManager& conn_manager;
        boost::asio::streambuf inbox, outbox;
    protected:
        Protocol *prot = nullptr;
        bool isWriting = false;

    };

    class TCPConnection : public Connection {
    public:
        TCPConnection(Server& sr);
        void receive() override;
        void sendData() override;
        void onConnect() override;
        bool isSSL() override;
        void disconnect() override;
        virtual boost::asio::ip::tcp::socket &getSocket();
        boost::asio::ip::tcp::socket peer;
    };

    class TLSConnection : public Connection {
    public:
        TLSConnection(Server& sr);
        void receive() override;
        void sendData() override;
        void onConnect() override;
        bool isSSL() override;
        void disconnect() override;
        virtual boost::asio::ip::tcp::socket &getSocket();
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket> peer;
    };

    class Server {
    public:
        Server(ServerManager& manager, std::string name, ProtocolType prot, boost::asio::ip::address address, uint16_t port,
               std::optional<boost::asio::ssl::context*> ssl_context);
        void listen();
        void start();
        void stop();
        ServerManager& srv_manager;
        ConnectionManager& conn_manager;
        boost::asio::io_context& context;
        std::optional<boost::asio::ssl::context*> ssl_con;
        bool running;
        std::string name;
        ProtocolType prot_type;
        boost::asio::ip::address addr;
        uint16_t port;
    private:
        boost::asio::ip::tcp::acceptor acceptor;

    };

    class ConnectionManager {
    public:
    private:
        std::unordered_map<std::string, Protocol*> protocols;
    };

    class ServerManager {
    public:
        ServerManager(boost::asio::io_context& io_con, ConnectionManager& conn_man);
        void createServer(std::string name, std::string address, uint16_t port, std::string protocol_name, std::optional<std::string> ssl_name);
        void registerSSL(std::string name);
        void registerAddress(std::string name, std::string addr);
        void start();
        void stop();
        boost::asio::io_context& context;
        ConnectionManager& conn_manager;
    private:
        std::unordered_map<std::string, Server*> servers;
        std::unordered_map<std::string, boost::asio::ip::address> addresses;
        std::unordered_map<std::string, boost::asio::ssl::context*> ssl_contexts;
        std::unordered_map<std::string, ProtocolType> protocols;

    };
}


#endif //FORGEPORTAL_PROTOCOL_H

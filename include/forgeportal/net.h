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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>


namespace forgeportal::net {
    class NetworkManager;
    class ServerManager;
    class ConnectionManager;
    class Server;
    class Connection;
    class Protocol;
    class TCPConnection;
    class TLSConnection;

    typedef Protocol*(*make_protocol_fn)();

    struct ProtocolFactory {
        make_protocol_fn create;
        std::string name;
    };

    class Protocol {
    public:
        explicit Protocol(boost::asio::io_context& con);
        virtual ~Protocol();
        virtual void setConnection(Connection *c);
        virtual void onReceiveData(boost::asio::streambuf& data) = 0;
        virtual void onClose() = 0;
        virtual void onLost() = 0;
        virtual void onConnect() = 0;
    protected:
        Connection *conn = nullptr;
    };
    
    class Connection {
    public:
        explicit Connection(Server& sr);
        virtual ~Connection();
        virtual void receive() = 0;
        virtual void sendData() = 0;
        virtual void disconnect() = 0;
        void onClose();
        void onLost();
        virtual void onConnect() = 0;
        virtual bool isSSL() = 0;
        virtual boost::asio::ip::tcp::socket &getSocket() = 0;
        void makeProtocol();
        std::vector<uint8_t> read_buffer;
        boost::asio::ip::address getAddress();
        boost::uuids::uuid id;
        boost::asio::io_context& context;
        Server &server;
        ServerManager& srv_manager;
        ConnectionManager& conn_manager;
        boost::asio::streambuf inbox, outbox;
        std::vector<uint8_t> outv, inv;
        bool active = false;
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
        boost::asio::ip::tcp::socket &getSocket() override;
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
        boost::asio::ip::tcp::socket &getSocket() override;
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
        ConnectionManager(NetworkManager &net_man);
    private:
        std::unordered_map<boost::uuids::uuid, Connection*> connections;
    };

    class ServerManager {
    public:
        ServerManager(NetworkManager &net_man);
    };

    class NetworkManager {
    public:
        NetworkManager(boost::asio::io_context& io_con, ConnectionManager& conn_man);
        void createServer(std::string name, std::string address, uint16_t port, std::string protocol_name, std::optional<std::string> ssl_name);
        void registerSSL(std::string name);
        void registerAddress(std::string name, std::string addr);
        void registerProtocol(std::string name, ProtocolFactory prot);
        void start();
        void stop();
        boost::asio::io_context& context;
        ServerManager& server_manager;
    private:
        std::unordered_map<std::string, Server*> servers;
        std::unordered_map<std::string, boost::asio::ip::address> addresses;
        std::unordered_map<std::string, boost::asio::ssl::context*> ssl_contexts;
        std::unordered_map<std::string, ProtocolFactory> protocols;
    };
}


#endif //FORGEPORTAL_PROTOCOL_H

//
// Created by volund on 11/27/20.
//

#include "forgeportal/net.h"
#include <iostream>
//#include "forgeportal/telnet.h"

namespace forgeportal::net {

    Protocol::Protocol(boost::asio::io_context& con) {}

    void Protocol::setConnection(Connection *c) {

    }

    Connection::Connection(Server &sr) : server(sr), context(sr.context), srv_manager(sr.srv_manager), conn_manager(sr.conn_manager) {}
    TCPConnection::TCPConnection(Server &sr) : Connection(sr), peer(sr.context) {}
    TLSConnection::TLSConnection(Server &sr) : Connection(sr), peer(sr.context, *sr.ssl_con.value()) {}
    boost::asio::ip::tcp::socket & TCPConnection::getSocket() {return peer;}
    boost::asio::ip::tcp::socket & TLSConnection::getSocket() {return peer.next_layer();}
    boost::asio::ip::address Connection::getAddress() {return getSocket().local_endpoint().address();}
    bool TCPConnection::isSSL() {return false;}
    bool TLSConnection::isSSL() {return true;}

    void TCPConnection::disconnect() {}
    void TLSConnection::disconnect() {}

    void Connection::makeProtocol() {
        switch(server.prot_type) {
            case ProtocolType::Telnet:
                //prot = new forgeportal::telnet::TelnetProtocol();
                break;
            case ProtocolType::HTTP:
                break;
            case ProtocolType::WebSocket:
                break;
        }
        if(prot) prot->setConnection(this);
    }

    void TLSConnection::onConnect() {
        peer.async_handshake(boost::asio::ssl::stream_base::server, [&](std::error_code ec){
            if(!ec) {
                makeProtocol();
                prot->onConnect();
            }
        });

    }

    void TCPConnection::onConnect() {
        makeProtocol();
        if(prot) prot->onConnect();
        receive();
    }

    void TCPConnection::sendData() {
        if(outbox.size() == 0) {
            isWriting = false;
        } else {
            isWriting = true;
            peer.async_write_some(outbox, [&](std::error_code ec, std::size_t len){
                outbox.consume(len);
                if(outbox.size() > 0) {
                    sendData();
                } else {
                    isWriting = false;
                }
            });
        }
    }

    void TLSConnection::sendData() {
        if(outbox.size() == 0) {
            isWriting = false;
        } else {
            isWriting = true;
            peer.async_write_some(outbox, [&](std::error_code ec, std::size_t len){
                outbox.consume(len);
                if(outbox.size() > 0) {
                    sendData();
                } else {
                    isWriting = false;
                }
            });
        }
    }

    void TCPConnection::receive() {
        peer.async_read_some(inbox, [&](std::error_code ec, std::size_t length) {
            if(!ec) {
                std::cout << "Got some bytes: " << length << std::endl << std::flush;
                if(prot) prot->onReceiveData(read_buffer, length);
                receive();
            } else
            {
                std::cout << "something went wrong" << std::endl << std::flush;
            }
        });
    }

    void TLSConnection::receive() {
        peer.async_read_some(inbox, [&](std::error_code ec, std::size_t length){
             if(!ec) {
                 std::cout << "Got some bytes: " << length << std::endl;
                 if(prot) prot->onReceiveData(read_buffer, length);
                 receive();
             }
         });
    }


    Server::Server(ServerManager& manager, std::string name, ProtocolType prot, boost::asio::ip::address address, uint16_t port,
                   std::optional<boost::asio::ssl::context*> ssl_context)
        : srv_manager(manager), conn_manager(manager.conn_manager), ssl_con(ssl_context),
        context(manager.context), acceptor(manager.context, boost::asio::ip::tcp::endpoint(address, port)) {
        prot_type = prot;
        this->name = name;
        addr = address;
        this->port = port;
        running = false;
    }

    void Server::start() {
        if(!running) {
            running = true;
            listen();
            return;
        }
    }

    void Server::stop() {
        if(running) {
            running = false;
        }
    }

    void Server::listen() {
        Connection *conn = nullptr;
        if(ssl_con.has_value()) {
            conn = new TLSConnection(*this);
        } else {
            conn = new TCPConnection(*this);
        }
        acceptor.async_accept(conn->getSocket(), [&](std::error_code ec) {
            if(!ec) {
                conn->onConnect();
            }
            if(running) {
                listen();
            }
        });
        return;
    }


    ServerManager::ServerManager(boost::asio::io_context& io_con, ConnectionManager& conn_man)
        : context(io_con), conn_manager(conn_man) {
        protocols.emplace(std::string("telnet"), ProtocolType::Telnet);
        protocols.emplace(std::string("http"), ProtocolType::HTTP);
        protocols.emplace(std::string("websocket"), ProtocolType::WebSocket);
    }

    void ServerManager::createServer(std::string name, std::string address, uint16_t port, std::string protocol_name,
                                     std::optional<std::string> ssl_name) {
        if(servers.contains(name)) {
            throw "duplicate server!";
        }
        if(!addresses.contains(address)) {
            throw "address not found";
        }
        auto a = addresses[address];

        if(!protocols.contains(protocol_name)) {
            throw "protocol not found";
        }
        auto p = protocols[protocol_name];

        std::optional<boost::asio::ssl::context*> s;
        if(ssl_name.has_value()) {
            if(!ssl_contexts.contains(ssl_name.value())) {
                throw "ssl context not found";
            }
            s.emplace(ssl_contexts[ssl_name.value()]);
        }
        servers[name] = new Server(*this, name, p, a, port, s);

    }

    void ServerManager::registerAddress(std::string name, std::string addr) {
        addresses.emplace(name, boost::asio::ip::make_address(addr));
    }

    void ServerManager::registerSSL(std::string name) {

    }

    void ServerManager::start() {
        for(const auto & [k, v] : servers) v->start();
    }

    void ServerManager::stop() {
        for(const auto & [k, v] : servers) v->stop();
    }

}
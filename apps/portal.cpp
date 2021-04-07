#include <iostream>
#include "forgeportal/net.h"

void got_cmd(std::string cmd) {
    std::cout << "got a command: " << cmd << std::endl;
}

int main() {
    boost::asio::io_context con;
    MudNetworkManager nm(con);

    nm.registerAddress(std::string("mine"), std::string("192.168.1.51"));

    std::optional<std::string> ssl;
    nm.registerListener(std::string("testing"), std::string("mine"), 7999, MudConnectionType::Telnet, ssl);
    nm.onConnectCB.emplace([&](auto conn) {
        std::cout << "got a connection ready!" << std::endl;
        conn->onCommandCB.emplace(&got_cmd);
    });
    nm.startListening();
    std::cout << "listening!" << std::endl;
    con.run();
    return 0;
}

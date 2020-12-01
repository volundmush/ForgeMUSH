//
// Created by volund on 11/27/20.
//

#ifndef FORGEMUSH_TELNET_H
#define FORGEMUSH_TELNET_H

#include <cstdint>
#include <unordered_set>
#include <unordered_map>
#include "forgeportal/net.h"

namespace forgeportal::telnet {
    class TelnetProtocol;
    class TelnetOption;
    class TelnetCodec;

    enum TelnetCode : uint8_t {
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

    enum TelnetMsgType : uint8_t {
        AppData = 0,
        Command = 1,
        Negotiation = 2,
        Subnegotiation = 3
    };

    class TelnetMessage : public net::Message {
    public:
        TelnetMsgType msg_type;
        std::vector<uint8_t> data;
        uint8_t codes[2];
    };


    class TelnetCodec : public net::Codec {
    public:
        void onReceiveData() override;
        void receiveFromProtocol(net::Message *msg) override;

    };



    enum TelnetState : uint8_t {
        Data = 0,
        Escaped = 1,
        Negotiating = 2,
        Subnegotiating = 3,
        Subescaped = 4,
        Endline = 5
    };

    struct TelnetOptionPerspective {
        bool enabled = false, negotiating = false, answered = false;
    };

    struct TelnetOptionState {
        TelnetOptionPerspective local, remote;
    };

    class TelnetOption {
    public:
        virtual TelnetCode opCode() = 0;
        virtual bool startWill(), startDo(), supportLocal(), supportRemote();
        virtual void registerHandshake();
        virtual void onConnect(), enableLocal(), enableRemote(), disableLocal(), disableRemote();
        void negotiate(TelnetCode command);
        void receiveNegotiate(TelnetCode command);
        virtual void rejectLocalHandshake(), acceptLocalHandshake(), rejectRemoteHandshake(), acceptRemoteHandshake();
        TelnetOptionState state;
        TelnetProtocol *protocol = nullptr;
    };

    class MXPOption : public TelnetOption {
    public:
        TelnetCode opCode() override;
    };

    class HandshakeHolder {
    public:
        HandshakeHolder(TelnetProtocol *p);
        std::unordered_set<TelnetCode> handshakes;
        void registerHandshake(TelnetCode code);
        void processHandshake(TelnetCode code);
        bool empty();
    private:
        TelnetProtocol *protocol = nullptr;
    };

    class TelnetProtocol : public forgeportal::net::Protocol {
    public:
        explicit TelnetProtocol(boost::asio::io_context& con);
        void onReceiveData(boost::asio::streambuf& data) override;
        void onClose() override;
        void onLost() override;
        void onConnect() override;
        void addHandler(TelnetOption* handler);
        void setConnection(net::Connection *c) override;
        HandshakeHolder handshake_local, handshake_remote, handshake_special;
        void sendNegotiate(TelnetCode command, uint8_t option);
        bool handshaken = false;
        void checkReady(), start();
    private:
        boost::asio::streambuf app_buffer;
        uint32_t overflow_counter = 0;
        TelnetState state = TelnetState::Data;
        std::unordered_map<uint8_t, TelnetOption*> handlers;
        boost::asio::high_resolution_timer timer;
    };

}

#endif //FORGEMUSH_TELNET_H

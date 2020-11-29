//
// Created by volund on 11/28/20.
//

#include "forgeportal/telnet.h"

namespace forgeportal::telnet {
    void TelnetOption::registerHandshake() {
        if(supportLocal() && startDo()) {
            protocol->handshake_local.registerHandshake(opCode());
        }
        if(supportRemote() && startWill()) {
            protocol->handshake_remote.registerHandshake(opCode());
        }
    }
    bool TelnetOption::startWill() {return false;}
    bool TelnetOption::startDo() {return false;}
    bool TelnetOption::supportLocal() {return false;}
    bool TelnetOption::supportRemote() {return false;}

    void TelnetOption::negotiate(TelnetCode command) {
        switch(command) {
            case TelnetCode::DO:
                state.remote.negotiating = true;
                protocol->sendNegotiate(command, opCode());
                break;
            case TelnetCode::WILL:
                state.local.negotiating = true;
                protocol->sendNegotiate(command, opCode());
                break;
            default:
                break;
        }
    }

    void TelnetOption::onConnect() {
        if(startWill()) {
            negotiate(TelnetCode::WILL);
        }
        if(startDo()) {
            negotiate(TelnetCode::DO);
        }
    }

    void TelnetOption::receiveNegotiate(TelnetCode command) {
        switch(command) {
            case TelnetCode::WILL:
                if(supportRemote()) {
                    if(state.remote.negotiating) {
                        state.remote.negotiating = false;
                        if(!state.remote.enabled) {
                            state.remote.enabled = true;
                            enableRemote();
                            if(!state.remote.answered) {
                                state.remote.answered = true;
                                protocol->handshake_remote.processHandshake(opCode());
                            }
                        }
                    } else {
                        state.remote.enabled = true;
                        protocol->sendNegotiate(TelnetCode::DO, opCode());
                        enableRemote();
                        if(!state.remote.answered) {
                            state.remote.answered = true;
                            protocol->handshake_remote.processHandshake(opCode());
                        }
                    }
                } else {
                    protocol->sendNegotiate(TelnetCode::DONT, opCode());
                }
                break;
            case TelnetCode::DO:
                if(supportLocal()) {
                    if(state.local.negotiating) {
                        state.local.negotiating = false;
                        if(!state.local.enabled) {
                            state.local.enabled = true;
                            enableLocal();
                            if(!state.local.answered) {
                                state.local.answered = true;
                                protocol->handshake_local.processHandshake(opCode());
                            }
                        }
                    } else {
                        state.local.enabled = true;
                        protocol->sendNegotiate(TelnetCode::WILL, opCode());
                        enableLocal();
                        if(!state.local.answered) {
                            state.local.answered = true;
                            protocol->handshake_local.processHandshake(opCode());
                        }
                    }
                } else {
                    protocol->sendNegotiate(TelnetCode::WONT, opCode());
                }
                break;
            case TelnetCode::WONT:
                if(state.remote.enabled) disableRemote();
                if(state.remote.negotiating) {
                    state.remote.negotiating = false;
                    if(!state.remote.answered) {
                        state.remote.answered = true;
                        protocol->handshake_remote.processHandshake(opCode());
                    }
                }
                break;
            case TelnetCode::DONT:
                if(state.local.enabled) disableLocal();
                if(state.local.negotiating) {
                    state.local.negotiating = false;
                    if(!state.local.answered) {
                        state.local.answered = true;
                        protocol->handshake_local.processHandshake(opCode());
                    }
                }
                break;
            default:
                break;
        }
    }

    HandshakeHolder::HandshakeHolder(TelnetProtocol *p) {
        protocol = p;
    }

    void HandshakeHolder::registerHandshake(TelnetCode code) {
        handshakes.insert(code);
    }

    void HandshakeHolder::processHandshake(TelnetCode code) {
        handshakes.erase(code);
        if(empty()) {
            protocol->checkReady();
        }
    }

    bool HandshakeHolder::empty() {return handshakes.empty();}

    void TelnetProtocol::addHandler(TelnetOption *handler) {
        handlers[handler->opCode()] = handler;
        handler->protocol = this;
        handler->registerHandshake();
    }

    TelnetProtocol::TelnetProtocol(boost::asio::io_context& con) : Protocol(con), timer(con), handshake_local(this), handshake_remote(this),
    handshake_special(this) {
        addHandler(new MXPOption);
    }

    TelnetCode MXPOption::opCode() {return TelnetCode::MXP;}



    void TelnetProtocol::onConnect() {
        timer.expires_after(std::chrono::milliseconds(500));
        for(const auto & [k, v] : handlers) {
            v->onConnect();
        }
        timer.async_wait([&](std::error_code ec) {
            start();
        });
    }

    void TelnetProtocol::checkReady() {
        if(!handshaken) {
            if(handshake_local.empty() && handshake_remote.empty() && handshake_special.empty()) {
                start();
            }
        }
    }

    void TelnetProtocol::start() {
        if(!handshaken) {
            handshaken = true; // set this so further negotiations won't trigger another 'start'
            // send signal to the ConnectionManager here...
        }
    }

    void TelnetProtocol::onReceiveData(std::vector<uint8_t> &data, size_t length) {
        for(const auto & b : data) {
            overflow_counter++;
            if(overflow_counter > 16 * 1024) {
                // NOPE! none of this tomfoolery.
            }

        }
    }

    void TelnetProtocol::onClose() {

    }

    void TelnetProtocol::onLost() {

    }

    void TelnetProtocol::sendNegotiate(TelnetCode command, uint8_t option) {
        auto a = conn->outbox.prepare(3);
        uint8_t c[3] = {TelnetCode::IAC, command, option};
        memcpy(a.data(), &c, 3);
        conn->outbox.commit(3);
        conn->sendData();
    };

}
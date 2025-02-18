#pragma once

#include "reactor-cpp/port.hh"

template <typename T>
void connect_(  std::set<reactor::Port<T>*> &left_ports, std::set<reactor::Port<T>*> &right_ports,
                reactor::ConnectionProperties &&property) {
    if (left_ports.size() < right_ports.size()) {
        reactor::log::Warn() << "There are more right ports (" << right_ports.size() << ") than left ports (" << left_ports.size() << ")";
        reactor::log::Warn() << "Left Ports:";
        for (auto *left_port : left_ports) {
            reactor::log::Warn() << left_port->fqn();
        }
        reactor::log::Warn() << "Right Ports:";
        for (auto *right_port : right_ports) {
            reactor::log::Warn() << right_port->fqn();
        }
    } else if (left_ports.size() > right_ports.size()) {
        reactor::log::Warn() << "There are more left ports (" << left_ports.size() << ") than right ports (" << right_ports.size() << ")";
        reactor::log::Warn() << "Left Ports:";
        for (auto *left_port : left_ports) {
            reactor::log::Warn() << left_port->fqn();
        }
        reactor::log::Warn() << "Right Ports:";
        for (auto *right_port : right_ports) {
            reactor::log::Warn() << right_port->fqn();
        }
    }

    auto right_port_itr = right_ports.begin();
    for (auto *left_port : left_ports) {
        if (right_port_itr == right_ports.end()) {
            break;
        }
        reactor::log::Warn() << "Left port:" << left_port->fqn() << " Right port:" << (*right_port_itr)->fqn();
        left_port->environment()->draw_connection(left_port, (*right_port_itr), reactor::ConnectionProperties{});
        ++right_port_itr;
    }
}

template <typename T>
void connect_fanout_(std::set<reactor::Port<T>*> &left_ports, std::set<reactor::Port<T>*> &right_ports,
                    reactor::ConnectionProperties &&property) {

}
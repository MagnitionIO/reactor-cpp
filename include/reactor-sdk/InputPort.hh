#pragma once

#include "reactor-cpp/reactor-cpp.hh"
// #include "Connection.hh"

namespace sdk
{

template <class T>
class Input : public reactor::Input<T> {
    class WiringProxy {
        public:
            WiringProxy(Input& origin) : origin(origin) {}

            void operator>(Input<T>& input) {
                origin.connect (input);
            }

            void operator>(MultiportInput<T>& input) {
                origin.connect_multiport (input);
            }

        private:
            Input& origin;
    };

    void connect(Input<T>& input);
    void connect_multiport(MultiportInput<T>& input);

public:
    using value_type = T;
    Input(const std::string& name, reactor::Reactor* container)
      : reactor::Input<T>(name, container) { /* cout << "input port created:" << name << " fqn:" << this->fqn() << " address:" << (void*)this << "\n";*/ }

    Input(Input&&) noexcept = default;
    ~Input() { /*cout << "input port destroyed fqn:" << this->fqn() << " address:" << (void*)this << "\n";*/ }

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }
};
    
} // namespace sdk

#include "impl/InputPort_wiring_impl.hh"
#pragma once

namespace sdk
{

template <typename T>
void Input<T>::connect(Input<T>& input) {
    this->environment()->draw_connection(*this, input, reactor::ConnectionProperties{});
}

template <typename T>
void Input<T>::connect_multiport(MultiportInput<T>& input) {
    auto input_itr = input.begin();

    if (1 < input.get_nports()) {
        reactor::log::Warn() << "Fanning out input to all right output ports";
    }

    while (input_itr != input.end())
    {
        this->environment()->draw_connection(*this, *input_itr, reactor::ConnectionProperties{});
        ++input_itr;
    }
}
    
} // namespace sdk

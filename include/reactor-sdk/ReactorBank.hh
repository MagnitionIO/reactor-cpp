#pragma once

namespace sdk
{

template<typename T>
class Input;

template<typename T>
class Output;

template<typename T>
class MultiportOutput;

template<typename T>
class MultiportInput;

template <typename ReactorType, typename T>
class ReactorBankInputPort {
public:
    ReactorBankInputPort(std::vector<std::unique_ptr<ReactorType>>& reactors, Input<T> ReactorType::*member)
        : reactors(reactors), member(member) {}
    
    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;
    
    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    Input<T> ReactorType::* get_member() { return member; }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    Input<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankInputPortOffset {
public:
    ReactorBankInputPortOffset(std::vector<std::unique_ptr<ReactorType>>& reactors, std::ptrdiff_t offset)
        : reactors(reactors), offset(offset) {}
    
    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;
    
    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    std::ptrdiff_t get_offset() { return offset; }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    std::ptrdiff_t offset;
};

template <typename ReactorType, typename T>
class ReactorBankInputMultiPort {
public:
    ReactorBankInputMultiPort(std::vector<std::unique_ptr<ReactorType>> &reactors, MultiportInput<T> ReactorType::*member)
        : reactors(reactors), member(member) {}

    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;

    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    MultiportInput<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankInputMultiPortOffset {
public:
    ReactorBankInputMultiPortOffset(std::vector<std::unique_ptr<ReactorType>>& reactors, std::ptrdiff_t offset)
        : reactors(reactors), offset(offset) {}
    
    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;
    
    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    std::ptrdiff_t get_offset() { return offset; }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    std::ptrdiff_t offset;
};

template <typename ReactorType, typename T>
class ReactorBankOutputPort {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputPort& origin) : origin(origin) {}

            void operator>(Input<T>& input) {
                origin.connect (input);
            }

            void operator>(MultiportInput<T>& input) {
                origin.connect (input);
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

        private:
            ReactorBankOutputPort& origin;
    };

    void connect(Input<T>& input);

    void connect(MultiportInput<T>& input);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports);

public:
    ReactorBankOutputPort(std::vector<std::unique_ptr<ReactorType>>& reactors, Output<T> ReactorType::*member)
    : reactors(reactors), member(member) {}

    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;

    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    Output<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankOutputPortOffset {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputPortOffset& origin) : origin(origin) {}

            void operator>(Input<T>& input) {
                origin.connect (input);
            }

            void operator>(MultiportInput<T>& input) {
                origin.connect (input);
            }

            void operator>>(MultiportInput<T>& input) {
                origin.connect_fanout (input);
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

        private:
            ReactorBankOutputPortOffset& origin;
    };

    void connect(Input<T>& input);
    void connect(MultiportInput<T>& input);
    void connect_fanout(MultiportInput<T>& input);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports);

public:
    ReactorBankOutputPortOffset(std::vector<std::unique_ptr<ReactorType>>& reactors, std::ptrdiff_t offset)
        : reactors(reactors), offset(offset) {}

    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;

    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    std::ptrdiff_t offset;
};

template <typename ReactorType, typename T>
class ReactorBankOutputMultiPort {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputMultiPort& origin) : origin(origin) {}

            void operator>(MultiportInput<T>& input) {
                origin.connect (input);
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

        private:
            ReactorBankOutputMultiPort& origin;
    };

    void connect(MultiportInput<T>& input);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports);

public:
    ReactorBankOutputMultiPort(std::vector<std::unique_ptr<ReactorType>>& reactors, MultiportOutput<T> ReactorType::*member)
        : reactors(reactors), member(member) {}

    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;

    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    MultiportOutput<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankOutputMultiPortOffset {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputMultiPortOffset& origin) : origin(origin) {}

            void operator>(MultiportInput<T>& input) {
                origin.connect (input);
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports) {
                origin.connect (std::move(other_bank_ports));
            }

        private:
            ReactorBankOutputMultiPortOffset& origin;
    };

    void connect(MultiportInput<T>& input);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports);

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports);

public:
    ReactorBankOutputMultiPortOffset(std::vector<std::unique_ptr<ReactorType>>& reactors, std::ptrdiff_t offset)
        : reactors(reactors), offset(offset) {}

    using iterator = typename std::vector<std::unique_ptr<ReactorType>>::iterator;
    using const_iterator = typename std::vector<std::unique_ptr<ReactorType>>::const_iterator;

    auto operator[](std::size_t index) noexcept -> ReactorType& { return *reactors[index]->get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { return *reactors[index]->get(); }

    auto begin() noexcept -> iterator { return reactors.begin(); };
    auto begin() const noexcept -> const_iterator { return reactors.begin(); };
    auto cbegin() const noexcept -> const_iterator { return reactors.cbegin(); };
    auto end() noexcept -> iterator { return reactors.end(); };
    auto end() const noexcept -> const_iterator { return reactors.end(); };
    auto cend() const noexcept -> const_iterator { return reactors.cend(); };

    auto size() const noexcept -> size_t { return reactors.size(); };
    [[nodiscard]] auto empty() const noexcept -> bool { return reactors.empty(); };

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    std::ptrdiff_t offset;
};

template <typename ReactorType>
class ReactorBank {
public:
    ReactorBank() = default;

    void reserve(std::size_t size) noexcept {
        reactors.reserve(size);
    }

    template <class... Args> void emplace_back(Args&&... args) noexcept {
        reactors.emplace_back(std::forward<Args>(args)...);
        reactors.back()->bank_index = index++;
    }

    template <typename T>
    std::pair<std::vector<std::unique_ptr<ReactorType>>*, Input<T> ReactorType::*> operator()(Input<T> ReactorType::*member) {
        return std::make_pair(&reactors, static_cast<Input<T> ReactorType::*>(member));
    }

    template <typename T>
    ReactorBankInputPort<ReactorType, T> operator->*(Input<T> ReactorType::*member) {
        return ReactorBankInputPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankInputPortOffset<ReactorType, T> operator->*(Input<T> *member) {
        std::size_t object_size = sizeof(ReactorType);
        std::ptrdiff_t offset = -1;

        for (auto &reactor : reactors) {
            const char* base_ptr = reinterpret_cast<const char*>(reactor.get());
            if ((reinterpret_cast<const char*>(member) >= base_ptr) && (reinterpret_cast<const char*>(member) < base_ptr + object_size)) {
                offset = reinterpret_cast<const char*>(member) - base_ptr;
                break;
            }
        }

        if (offset < 0) {
            std::cerr << "Member passed in is not a valid member\n";
            reactor_assert(false);
        }
        
        return ReactorBankInputPortOffset<ReactorType, T>(reactors, offset);
    }

    template <typename T>
    ReactorBankInputMultiPort<ReactorType, T> operator->*(MultiportInput<T> ReactorType::*member) {
        return ReactorBankInputMultiPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankInputMultiPortOffset<ReactorType, T> operator->*(MultiportInput<T> *member) {
        std::size_t object_size = sizeof(ReactorType);
        std::ptrdiff_t offset = -1;

        for (auto &reactor : reactors) {
            const char* base_ptr = reinterpret_cast<const char*>(reactor.get());
            if ((reinterpret_cast<const char*>(member) >= base_ptr) && (reinterpret_cast<const char*>(member) < base_ptr + object_size)) {
                offset = reinterpret_cast<const char*>(member) - base_ptr;
                break;
            }
        }

        if (offset < 0) {
            std::cerr << "Member passed in is not a valid member\n";
            reactor_assert(false);
        }

        return ReactorBankInputMultiPortOffset<ReactorType, T>(reactors, offset);
    }

    template <typename T>
    ReactorBankOutputPort<ReactorType, T> operator->*(Output<T> ReactorType::*member) {
        return ReactorBankOutputPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankOutputPortOffset<ReactorType, T> operator->*(Output<T> *member) {
        std::size_t object_size = sizeof(ReactorType);
        std::ptrdiff_t offset = -1;

        for (auto &reactor : reactors) {
            const char* base_ptr = reinterpret_cast<const char*>(reactor.get());
            if ((reinterpret_cast<const char*>(member) >= base_ptr) && (reinterpret_cast<const char*>(member) < base_ptr + object_size)) {
                offset = reinterpret_cast<const char*>(member) - base_ptr;
                break;
            }
        }

        if (offset < 0) {
            std::cerr << "Member passed in is not a valid member\n";
            reactor_assert(false);
        }

        return ReactorBankOutputPortOffset<ReactorType, T>(reactors, offset);
    }

    template <typename T>
    ReactorBankOutputMultiPort<ReactorType, T> operator->*(MultiportOutput<T> ReactorType::*member) {
        return ReactorBankOutputMultiPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankOutputMultiPortOffset<ReactorType, T> operator->*(MultiportOutput<T> *member) {
        std::size_t object_size = sizeof(ReactorType);
        std::ptrdiff_t offset = -1;

        for (auto &reactor : reactors) {
            const char* base_ptr = reinterpret_cast<const char*>(reactor.get());
            if ((reinterpret_cast<const char*>(member) >= base_ptr) && (reinterpret_cast<const char*>(member) < base_ptr + object_size)) {
                offset = reinterpret_cast<const char*>(member) - base_ptr;
                break;
            }
        }

        if (offset < 0) {
            std::cerr << "Member passed in is not a valid member\n";
            reactor_assert(false);
        }

        return ReactorBankOutputMultiPortOffset<ReactorType, T>(reactors, offset);
    }

    ReactorBank& operator->() {
        return *this;
    }

    auto operator[](std::size_t index) noexcept -> ReactorType& { assert (index < reactors.size()); return *reactors[index].get(); }
    auto operator[](std::size_t index) const noexcept -> const ReactorType& { assert (index < reactors.size()); return *reactors[index].get(); }

private:
    std::vector<std::unique_ptr<ReactorType>> reactors;
    size_t index = 0;
};

    
} // namespace sdk

#include "impl/ReactorBankOutputPort_wiring_impl.hh"
#include "impl/ReactorBankOutputMultiport_wiring_impl.hh"
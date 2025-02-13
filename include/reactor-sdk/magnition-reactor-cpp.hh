#pragma once

#include <type_traits>
#include <functional>
#include <unordered_map>
#include <string>
#include <any>
#include <cxxabi.h>
// #include <yaml-cpp/yaml.h>

#include "reactor-cpp/reactor-cpp.hh"

using namespace std;

template <typename Fn, typename InputTuple, typename OutputTuple>
class MagnitionReaction;

class MagnitionReactor;

template<typename T>
class input_t;

template<typename T>
class output_t;

template<class T>
class multiport_output_t;

template<class T>
class multiport_input_t;

extern std::map<std::string, std::string> type_convert;

template <typename T>
struct inspect_function_args;

template <typename Ret, typename Class, typename... Args>
struct inspect_function_args<Ret(Class::*)(Args...)> {
    static constexpr size_t nargs = sizeof...(Args);
};

template <typename Func, typename Object>
auto bind_function(Object* obj, Func&& func) {
    constexpr size_t nargs = inspect_function_args<Func>::nargs;

    if constexpr (nargs == 0) {
        static_assert(nargs > 0, "Reactors must have one or more parameters");
        return nullptr;
    } else if constexpr (nargs == 1) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1);
    } else if constexpr (nargs == 2) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2);
    } else if constexpr (nargs == 3) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    } else if constexpr (nargs == 4) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    } else if constexpr (nargs == 5) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    } else if constexpr (nargs == 6) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
    } else if constexpr (nargs == 7) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7);
    } else if constexpr (nargs == 8) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8);
    } else if constexpr (nargs == 9) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9);
    } else if constexpr (nargs == 10) {
        return std::bind(std::forward<Func>(func), obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10);
    } else {
        static_assert(nargs <= 10, "This needs to be extended as per requirement of more parameters");
        return nullptr;
    }

}

#define pass_function(func) \
    bind_function(this, &std::decay_t<decltype(*this)>::func)

template <typename T>
struct strip_smart_pointer {
    using type = T;
};

template <typename T>
struct strip_smart_pointer<std::unique_ptr<T>> {
    using type = T;
};

template <typename T>
using strip_smart_pointer_t = typename strip_smart_pointer<T>::type;

#define OUTPUTS(...) std::make_tuple(__VA_ARGS__)
#define INPUTS(...) std::make_tuple(__VA_ARGS__)
#define BANKED_INPUTS(vector, member) \
    std::make_pair(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)

template <typename T, template <typename...> class Template>
struct is_specialization : std::false_type
{
};

template <typename... Args, template <typename...> class Template>
struct is_specialization<Template<Args...>, Template> : std::true_type
{
};

template <typename T, template <typename...> class Template>
inline constexpr bool is_specialization_v = is_specialization<T, Template>::value;

template <typename ReactorType, typename T>
class ReactorBankInputPort {
public:
    ReactorBankInputPort(std::vector<std::unique_ptr<ReactorType>>& reactors, input_t<T> ReactorType::*member)
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

    input_t<T> ReactorType::* get_member() { return member; }



private:
    std::vector<std::unique_ptr<ReactorType>>& reactors;
    input_t<T> ReactorType::*member;
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
    ReactorBankInputMultiPort(std::vector<std::unique_ptr<ReactorType>> &reactors, multiport_input_t<T> ReactorType::*member)
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
    multiport_input_t<T> ReactorType::*member;
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

            void operator>(input_t<T>& input) {
                origin.connect (input);
            }

            void operator>(multiport_input_t<T>& input) {
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

    void connect(input_t<T>& input) {
        auto reactor_itr = reactors.begin();

        if (1 < reactors.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto *reactor = (*reactor_itr).get();
        input.environment()->draw_connection(reactor->*member, input, reactor::ConnectionProperties{});
    }

    void connect(multiport_input_t<T>& input) {
        auto reactor_itr = reactors.begin();

        if (input.get_nports() > reactors.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (input.get_nports() < reactors.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& input_port : input) {
            auto *reactor = (*reactor_itr).get();
            input_port.environment()->draw_connection(reactor->*member, input_port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = other_bank_ports.size();

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : other_bank_ports) {
            auto *reactor = (*reactor_itr).get();
            auto *right_reactor = p_right_reactor.get();
            (reactor->*member).environment()->draw_connection(reactor->*member, right_reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = 0;

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*(other_bank_ports.get_member())).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            if (reactor_itr == reactors.end())
            {
                break;
            }
            
            for (auto& right_port : right_reactor->*(other_bank_ports.get_member())) {
                auto *reactor = (*reactor_itr).get();
                (reactor->*member).environment()->draw_connection(reactor->*member, right_port, reactor::ConnectionProperties{});
                ++reactor_itr;
                if (reactor_itr == reactors.end())
                {
                    break;
                }
            }
        }
    }

public:
    ReactorBankOutputPort(std::vector<std::unique_ptr<ReactorType>>& reactors, output_t<T> ReactorType::*member)
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
    output_t<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankOutputPortOffset {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputPortOffset& origin) : origin(origin) {}

            void operator>(input_t<T>& input) {
                origin.connect (input);
            }

            void operator>(multiport_input_t<T>& input) {
                origin.connect (input);
            }

            void operator>>(multiport_input_t<T>& input) {
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

    void connect(input_t<T>& input) {
        auto reactor_itr = reactors.begin();

        if (1 < reactors.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto *reactor = (*reactor_itr).get();
        char* reactor_base = reinterpret_cast<char*>(reactor);
        output_t<T>* port = reinterpret_cast<output_t<T>*>(reactor_base + offset);
        input.environment()->draw_connection(*port, input, reactor::ConnectionProperties{});
    }

    void connect(multiport_input_t<T>& input) {
        auto reactor_itr = reactors.begin();

        if (input.get_nports() > reactors.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (input.get_nports() < reactors.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& input_port : input) {
            auto *reactor = (*reactor_itr).get();
            char* reactor_base = reinterpret_cast<char*>(reactor);
            output_t<T>* port = reinterpret_cast<output_t<T>*>(reactor_base + offset);
            input_port.environment()->draw_connection(*port, input_port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                break;
            }
        }
    }

    void connect_fanout(multiport_input_t<T>& input) {
        auto reactor_itr = reactors.begin();

        if (input.get_nports() > reactors.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Fanning Out!";
        } else if (input.get_nports() < reactors.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& input_port : input) {
            auto *reactor = (*reactor_itr).get();
            char* reactor_base = reinterpret_cast<char*>(reactor);
            output_t<T>* port = reinterpret_cast<output_t<T>*>(reactor_base + offset);
            input_port.environment()->draw_connection(*port, input_port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                reactor_itr = reactors.begin();
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = other_bank_ports.size();

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : other_bank_ports) {
            auto *reactor = (*reactor_itr).get();
            char* l_reactor_base = reinterpret_cast<char*>(reactor);
            output_t<T>* l_port = reinterpret_cast<output_t<T>*>(l_reactor_base + offset);
            auto *right_reactor = p_right_reactor.get();
            (*l_port).environment()->draw_connection(*l_port, right_reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = 0;

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*(other_bank_ports.get_member())).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            if (reactor_itr == reactors.end())
            {
                break;
            }
            
            for (auto& right_port : right_reactor->*(other_bank_ports.get_member())) {
                auto *reactor = (*reactor_itr).get();
                char* l_reactor_base = reinterpret_cast<char*>(reactor);
                output_t<T>* l_port = reinterpret_cast<output_t<T>*>(l_reactor_base + offset);
                (*l_port).environment()->draw_connection(*l_port, right_port, reactor::ConnectionProperties{});
                ++reactor_itr;
                if (reactor_itr == reactors.end())
                {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = other_bank_ports.size();

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : other_bank_ports) {
            auto *reactor = (*reactor_itr).get();
            char* l_reactor_base = reinterpret_cast<char*>(reactor);
            output_t<T>* l_port = reinterpret_cast<output_t<T>*>(l_reactor_base + offset);
            auto *right_reactor = p_right_reactor.get();
            char* r_reactor_base = reinterpret_cast<char*>(right_reactor);
            input_t<T>* r_port = reinterpret_cast<input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
            (*l_port).environment()->draw_connection(*l_port, *r_port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors.end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = reactors.size();
        size_t right_ports = 0;

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            char* r_reactor_base = reinterpret_cast<char*>(right_reactor);
            multiport_input_t<T>* r_port = reinterpret_cast<multiport_input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
            right_ports += (*r_port).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            if (reactor_itr == reactors.end())
            {
                break;
            }
            
            char* r_reactor_base = reinterpret_cast<char*>(right_reactor);
            multiport_input_t<T>* r_port = reinterpret_cast<multiport_input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
            for (auto& right_port : *r_port) {
                auto *reactor = (*reactor_itr).get();
                char* l_reactor_base = reinterpret_cast<char*>(reactor);
                output_t<T>* l_port = reinterpret_cast<output_t<T>*>(l_reactor_base + offset);
                (*l_port).environment()->draw_connection(*l_port, right_port, reactor::ConnectionProperties{});
                ++reactor_itr;
                if (reactor_itr == reactors.end())
                {
                    break;
                }
            }
        }
    }

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

            void operator>(multiport_input_t<T>& input) {
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

    void connect(multiport_input_t<T>& input) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = input.get_nports();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_port_itr = input.begin();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_itr == input.end()) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_itr;
                if (right_port_itr == input.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = other_bank_ports.size();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_reactor_itr == other_bank_ports.end()) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                auto *right_reactor = (*right_reactor_itr).get();
                l_port.environment()->draw_connection(l_port, right_reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
                ++right_reactor_itr;
                if (right_reactor_itr == other_bank_ports.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*(other_bank_ports.get_member())).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();
        auto right_port_itr =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).begin();
        auto right_port_itr_end =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).end();
        size_t right_port_count = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_count == right_ports) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_count;
                if (right_port_count == right_ports) {
                    break;
                }
                ++right_port_itr;
                if (right_port_itr == right_port_itr_end) {
                    ++right_reactor_itr;
                    right_port_itr =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).begin();
                    right_port_itr_end =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).end();
                }
            }
        }
    }

public:
    ReactorBankOutputMultiPort(std::vector<std::unique_ptr<ReactorType>>& reactors, multiport_output_t<T> ReactorType::*member)
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
    multiport_output_t<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class ReactorBankOutputMultiPortOffset {
    class WiringProxy {
        public:
            WiringProxy(ReactorBankOutputMultiPortOffset& origin) : origin(origin) {}

            void operator>(multiport_input_t<T>& input) {
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

    void connect(multiport_input_t<T>& input) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = input.get_nports();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_port = reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            left_ports += (*l_port).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_port_itr = input.begin();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_itr == input.end()) {
                break;
            }

            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            for (auto &l_port : *l_ports) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_itr;
                if (right_port_itr == input.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = other_bank_ports.size();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            left_ports += (*l_ports).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_reactor_itr == other_bank_ports.end()) {
                break;
            }

            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            for (auto &l_port : *l_ports) {
                auto *right_reactor = (*right_reactor_itr).get();
                l_port.environment()->draw_connection(l_port, right_reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
                ++right_reactor_itr;
                if (right_reactor_itr == other_bank_ports.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            left_ports += (*l_ports).get_nports();
        }

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*(other_bank_ports.get_member())).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();
        auto right_port_itr =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).begin();
        auto right_port_itr_end =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).end();
        size_t right_port_count = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_count == right_ports) {
                break;
            }

            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            for (auto &l_port : *l_ports) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_count;
                if (right_port_count == right_ports) {
                    break;
                }
                ++right_port_itr;
                if (right_port_itr == right_port_itr_end) {
                    ++right_reactor_itr;
                    right_port_itr =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).begin();
                    right_port_itr_end =  (*right_reactor_itr).get()->*(other_bank_ports.get_member()).end();
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = other_bank_ports.size();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            left_ports += (*l_ports).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_reactor_itr == other_bank_ports.end()) {
                break;
            }

            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            for (auto &l_port : *l_ports) {
                auto *right_reactor = (*right_reactor_itr).get();
                char* r_reactor_base = reinterpret_cast<char*>(right_reactor);
                input_t<T>* r_port= reinterpret_cast<input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
                l_port.environment()->draw_connection(l_port, *r_port, reactor::ConnectionProperties{});
                ++right_reactor_itr;
                if (right_reactor_itr == other_bank_ports.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputMultiPortOffset<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = reactors.begin();
        size_t left_ports = 0;
        size_t right_ports = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();
            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            left_ports += (*l_ports).get_nports();
        }

        for (auto& p_right_reactor : other_bank_ports) {
            auto *right_reactor = p_right_reactor.get();
            char* r_reactor_base = reinterpret_cast<char*>(right_reactor);
            multiport_input_t<T>* r_ports = reinterpret_cast<multiport_input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
            right_ports += (*r_ports).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_bank_ports.begin();
        char* r_reactor_base = reinterpret_cast<char*>(*right_reactor_itr);
        multiport_input_t<T>* r_ports = reinterpret_cast<multiport_input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
        auto right_port_itr =  (*r_ports).begin();
        auto right_port_itr_end =  (*r_ports).end();
        size_t right_port_count = 0;

        for (auto& p_left_reactor : reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_count == right_ports) {
                break;
            }

            char* l_reactor_base = reinterpret_cast<char*>(left_reactor);
            multiport_output_t<T>* l_ports= reinterpret_cast<multiport_output_t<T>*>(l_reactor_base + offset);
            for (auto &l_port : *l_ports) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_count;
                if (right_port_count == right_ports) {
                    break;
                }
                ++right_port_itr;
                if (right_port_itr == right_port_itr_end) {
                    ++right_reactor_itr;
                    r_reactor_base = reinterpret_cast<char*>(*right_reactor_itr);
                    r_ports = reinterpret_cast<multiport_input_t<T>*>(r_reactor_base + other_bank_ports.get_offset());
                    right_port_itr =  (*r_ports).begin();
                    right_port_itr_end =  (*r_ports).end();
                }
            }
        }
    }

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
    std::pair<std::vector<std::unique_ptr<ReactorType>>*, input_t<T> ReactorType::*> operator()(input_t<T> ReactorType::*member) {
        return std::make_pair(&reactors, static_cast<input_t<T> ReactorType::*>(member));
    }

    template <typename T>
    ReactorBankInputPort<ReactorType, T> operator->*(input_t<T> ReactorType::*member) {
        return ReactorBankInputPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankInputPortOffset<ReactorType, T> operator->*(input_t<T> *member) {
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
    ReactorBankInputMultiPort<ReactorType, T> operator->*(multiport_input_t<T> ReactorType::*member) {
        return ReactorBankInputMultiPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankInputMultiPortOffset<ReactorType, T> operator->*(multiport_input_t<T> *member) {
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
    ReactorBankOutputPort<ReactorType, T> operator->*(output_t<T> ReactorType::*member) {
        return ReactorBankOutputPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankOutputPortOffset<ReactorType, T> operator->*(output_t<T> *member) {
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
    ReactorBankOutputMultiPort<ReactorType, T> operator->*(multiport_output_t<T> ReactorType::*member) {
        return ReactorBankOutputMultiPort<ReactorType, T>(reactors, member);
    }

    template <typename T>
    ReactorBankOutputMultiPortOffset<ReactorType, T> operator->*(multiport_output_t<T> *member) {
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

#define select_default(obj) &obj[0]

template<class T>
class multiport_input_t : public reactor::ModifableMultiport<input_t<T>> {
    size_t n_inputs;
    string name;
    MagnitionReactor *reactor;

    class WiringProxy {
        public:
            WiringProxy(multiport_input_t& origin) : origin(origin) {}

            void operator>(input_t<T>& input) {
                origin.connect (input);
            }

            void operator>(multiport_input_t<T>& input) {
                origin.connect (input);
            }

        private:
            multiport_input_t& origin;
    };

    void connect(input_t<T>& input) {
        if (n_inputs > 1) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, input, reactor::ConnectionProperties{});
            break;
        }
    }

    void connect(multiport_input_t<T>& input) {
        auto input_itr = input.begin();

        if (n_inputs < input.get_nports()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > input.get_nports()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, *input_itr, reactor::ConnectionProperties{});
            ++input_itr;
            if (input_itr == input.end())
            {
                break;
            }
        }
    }

public:
    using value_type = T;
    multiport_input_t(const std::string& name, MagnitionReactor* container)
        : name (name), reactor (container) {}

    void set_width (int width)
    {
        this->reserve(width);
        n_inputs = width;
        for (int idx = 0; idx < width; idx++) {
            std::string input_name = name + "_" + std::to_string(idx);
            this->emplace_back(input_name, reactor);
        }
    }

    multiport_input_t(multiport_input_t&&) noexcept = default;
    auto get_nports() -> int { return n_inputs; }

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }
};

template<class T>
class multiport_output_t : public reactor::ModifableMultiport<output_t<T>> {
    size_t n_inputs;
    string name;
    MagnitionReactor *reactor;
    class WiringProxy {
    public:
        WiringProxy(multiport_output_t& origin) : origin(origin) {}

        void operator>(input_t<T>& input) {
            origin.connect (input);
        }

        void operator>(output_t<T>& input) {
            origin.connect (input);
        }

        void operator>(multiport_input_t<T>& input) {
            origin.connect (input);
        }

        void operator>(multiport_output_t<T>& input) {
            origin.connect (input);
        }

        template <typename ReactorType>
        void operator>(std::pair<std::vector<std::unique_ptr<ReactorType>>*, input_t<T> ReactorType::*> connections)
        {
            origin.connect (connections.first, connections.second);
        }

        template <typename OtherReactorType>
        void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
            origin.connect(std::move(other_bank_ports));
        }

        template <typename OtherReactorType>
        void operator>(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
            origin.connect(std::move(other_bank_ports));
        }

    private:
        multiport_output_t& origin;
    };

    void connect(input_t<T>& input) {
        if (n_inputs > 1) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, input, reactor::ConnectionProperties{});
            break;
        }
    }

    void connect(output_t<T>& input) {
        if (n_inputs > 1) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, input, reactor::ConnectionProperties{});
            break;
        }
    }

    void connect(multiport_input_t<T>& input) {
        auto input_itr = input.begin();

        if (n_inputs < input.get_nports()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > input.get_nports()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, *input_itr, reactor::ConnectionProperties{});
            ++input_itr;
            if (input_itr == input.end())
            {
                break;
            }
        }
    }

    void connect(multiport_output_t<T>& input) {
        auto input_itr = input.begin();

        if (n_inputs < input.get_nports()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > input.get_nports()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        for (auto& output_port : *this) {
            output_port.environment()->draw_connection(output_port, *input_itr, reactor::ConnectionProperties{});
            ++input_itr;
            if (input_itr == input.end())
            {
                break;
            }
        }
    }

    template <typename ReactorType>
    void connect(std::vector<std::unique_ptr<ReactorType>>* reactors, input_t<T> ReactorType::*member) {
        auto reactor_itr = reactors->begin();

        if (n_inputs < reactors->size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > reactors->size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            auto *reactor = (*reactor_itr).get();
            output_port.environment()->draw_connection(output_port, reactor->*member, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors->end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = other_bank_ports.begin();

        if (n_inputs < other_bank_ports.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > other_bank_ports.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            auto *reactor = (*reactor_itr).get();
            output_port.environment()->draw_connection(output_port, reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == other_bank_ports.end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void connect(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
        auto reactor_itr = other_bank_ports.begin();

        if (n_inputs < other_bank_ports.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (n_inputs > other_bank_ports.size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& output_port : *this) {
            auto *reactor = (*reactor_itr).get();
            char* reactor_base = reinterpret_cast<char*>(reactor);
            input_t<T>* port = reinterpret_cast<input_t<T>*>(reactor_base + other_bank_ports.get_offset());
            output_port.environment()->draw_connection(output_port, *port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == other_bank_ports.end())
            {
                break;
            }
        }
    }

public:
    using value_type = T;
    multiport_output_t(const std::string& name, MagnitionReactor* container)
        : name (name), reactor (container) {}

    void set_width (int width)
    {
        this->reserve(width);
        n_inputs = width;
        for (int idx = 0; idx < width; idx++) {
            std::string input_name = name + "_" + std::to_string(idx);
            this->emplace_back(input_name, reactor);
        }
    }

    multiport_output_t(multiport_output_t&&) noexcept = default;
    auto get_nports() -> int { return n_inputs; }

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }
};

template <class T>
class output_t : public reactor::Output<T> {
    std::set<output_t<T>*> accumulated;
    bool is_accumulated = false;

    class WiringProxy {
        public:
            WiringProxy(output_t& origin) : origin(origin) {}

            void operator>(input_t<T>& input) {
                origin.connect (input);
            }

            void operator>(output_t<T>& input) {
                origin.connect (input);
            }

            void operator>(multiport_input_t<T>& input) {
                origin.connect_multiport (input);
            }

            template <typename ReactorType>
            void operator>(std::pair<std::vector<std::unique_ptr<ReactorType>>*, input_t<T> ReactorType::*> connections)
            {
                origin.connect_multiport (connections.first, connections.second);
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPort<OtherReactorType, T> &&other_bank_ports) {
                origin.connect_multiport(std::move(other_bank_ports));
            }

            template <typename OtherReactorType>
            void operator>(ReactorBankInputPortOffset<OtherReactorType, T> &&other_bank_ports) {
                origin.connect_multiport(std::move(other_bank_ports));
            }

        private:
            output_t& origin;
    };

    void connect(input_t<T>& input) {
        this->environment()->draw_connection(*this, input, reactor::ConnectionProperties{});
    }

    void connect(output_t<T>& input) {
        this->environment()->draw_connection(*this, input, reactor::ConnectionProperties{});
    }

    void connect_multiport(multiport_input_t<T>& input) {
        if (is_accumulated) {
            auto input_itr = input.begin();

            if (accumulated.size() < input.get_nports()) {
                reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
            } else if (accumulated.size() > input.get_nports()) {
                reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
            }

            this->environment()->draw_connection(*this, *input_itr, reactor::ConnectionProperties{});
            ++input_itr;

            for (auto *l_port : accumulated) {
                if (input_itr != input.end()) {
                    break;
                }
                this->environment()->draw_connection(*l_port, *input_itr, reactor::ConnectionProperties{});
                ++input_itr;
            }
            is_accumulated = false;
            accumulated.clear();
        } else {
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
    }

    template <typename ReactorType>
    void connect_multiport(std::vector<std::unique_ptr<ReactorType>>* reactors, input_t<T> ReactorType::*member) {

        if (1 < reactors->size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        }
        for (auto &p_reactor : *reactors) {
            auto *reactor = p_reactor.get();
            this->environment()->draw_connection(*this, reactor->*member, reactor::ConnectionProperties{});
        }
    }

    template <typename ReactorType>
    void connect_multiport(ReactorBankInputPort<ReactorType, T> &&other_bank_ports) {

        if (1 < other_bank_ports.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        }
        for (auto &p_reactor : other_bank_ports) {
            auto *reactor = p_reactor.get();
            this->environment()->draw_connection(*this, reactor->*(other_bank_ports.get_member()), reactor::ConnectionProperties{});
        }
    }

    template <typename ReactorType>
    void connect_multiport(ReactorBankInputPortOffset<ReactorType, T> &&other_bank_ports) {

        if (1 < other_bank_ports.size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        }
        for (auto &p_reactor : other_bank_ports) {
            auto *reactor = p_reactor.get();
            char* reactor_base = reinterpret_cast<char*>(reactor);
            input_t<T>* port = reinterpret_cast<input_t<T>*>(reactor_base + other_bank_ports.get_offset());
            this->environment()->draw_connection(*this, *port, reactor::ConnectionProperties{});
        }
    }
    

public:
    using value_type = T;
    output_t(const std::string& name, reactor::Reactor* container)
      : reactor::Output<T>(name, container) {/* cout << "output port created:" << name << " fqn:" << this->fqn() << " address:" << (void*)this << "\n"; */}
    
    ~output_t() { /* cout << "output port destroyed fqn:" << this->fqn() << " address:" << (void*)this << "\n"; */}

    output_t(output_t&&) noexcept = default;

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }

    output_t<T>& operator+(output_t<T> &output) {
        [[maybe_unused]] bool result = accumulated.insert(&output).second;
        reactor_assert(result);
        is_accumulated = true;
        return *this;
    }
};

template <class T>
class input_t : public reactor::Input<T> {
    class WiringProxy {
        public:
            WiringProxy(input_t& origin) : origin(origin) {}

            void operator>(input_t<T>& input) {
                origin.connect (input);
            }

            void operator>(multiport_input_t<T>& input) {
                origin.connect_multiport (input);
            }

        private:
            input_t& origin;
    };

    void connect(input_t<T>& input) {
        this->environment()->draw_connection(*this, input, reactor::ConnectionProperties{});
    }

    void connect_multiport(multiport_input_t<T>& input) {
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

public:
    using value_type = T;
    input_t(const std::string& name, reactor::Reactor* container)
      : reactor::Input<T>(name, container) { /* cout << "input port created:" << name << " fqn:" << this->fqn() << " address:" << (void*)this << "\n";*/ }

    input_t(input_t&&) noexcept = default;
    ~input_t() { /*cout << "input port destroyed fqn:" << this->fqn() << " address:" << (void*)this << "\n";*/ }

    WiringProxy operator--(int) {
        return WiringProxy(*this);
    }
};

template <typename ReactorType, typename T>
class BankedOutputConnector {
public:
    BankedOutputConnector(std::vector<std::unique_ptr<ReactorType>>* vector, output_t<T> ReactorType::*member)
        : reactors(vector), member(member) {}

    void operator>(multiport_input_t<T>& input) {
        auto reactor_itr = reactors->begin();

        if (input.get_nports() > reactors->size()) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (input.get_nports() < reactors->size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& input_port : input) {
            auto *reactor = (*reactor_itr).get();
            input_port.environment()->draw_connection(reactor->*member, input_port, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors->end())
            {
                break;
            }
        }
    }

    void operator>(input_t<T>& input) {
        auto reactor_itr = reactors->begin();

        if (1 < reactors->size()) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto *reactor = (*reactor_itr).get();
        input.environment()->draw_connection(reactor->*member, input, reactor::ConnectionProperties{});
    }

    template <typename OtherReactorType>
    void connect_ports(std::vector<std::unique_ptr<OtherReactorType>>* other_reactors, input_t<T> OtherReactorType::*other_member) {
        auto reactor_itr = reactors->begin();
        size_t left_ports = reactors->size();
        size_t right_ports = other_reactors->size();

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : *other_reactors) {
            auto *reactor = (*reactor_itr).get();
            auto *right_reactor = p_right_reactor.get();
            (reactor->*member).environment()->draw_connection(reactor->*member, right_reactor->*other_member, reactor::ConnectionProperties{});
            ++reactor_itr;
            if (reactor_itr == reactors->end())
            {
                break;
            }
        }
    }

    template <typename OtherReactorType>
    void operator>(std::pair<std::vector<std::unique_ptr<OtherReactorType>>*, input_t<T> OtherReactorType::*> connections)
    {
        this->connect_ports (connections.first, connections.second);
    }

    template <typename OtherReactorType>
    void connect_ports(std::vector<std::unique_ptr<OtherReactorType>>* other_reactors, multiport_input_t<T> OtherReactorType::*other_member) {
        auto reactor_itr = reactors->begin();
        size_t left_ports = reactors->size();
        size_t right_ports = 0;

        for (auto& p_right_reactor : *other_reactors) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*other_member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }
        for (auto& p_right_reactor : *other_reactors) {
            auto *reactor = (*reactor_itr).get();
            auto *right_reactor = p_right_reactor.get();

            if (reactor_itr == reactors->end())
            {
                break;
            }
            
            for (auto& right_port : right_reactor->*other_member) {
                (reactor->*member).environment()->draw_connection(reactor->*member, right_port, reactor::ConnectionProperties{});
                ++reactor_itr;
                if (reactor_itr == reactors->end())
                {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void operator>(std::pair<std::vector<std::unique_ptr<OtherReactorType>>*, multiport_input_t<T> OtherReactorType::*> connections)
    {
        this->connect_ports (connections.first, connections.second);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>* reactors;
    output_t<T> ReactorType::*member;
};

template <typename ReactorType, typename T>
class BankedMultiportOutputConnector {
public:
    BankedMultiportOutputConnector(std::vector<std::unique_ptr<ReactorType>>* vector, multiport_output_t<T> ReactorType::*member)
        : reactors(vector), member(member) {}

    void operator>(multiport_input_t<T>& input) {
        auto reactor_itr = reactors->begin();
        size_t left_ports = 0;
        size_t right_ports = input.get_nports();

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_port_itr = input.begin();

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_itr == input.end()) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_itr;
                if (right_port_itr == input.end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void connect_ports(std::vector<std::unique_ptr<OtherReactorType>>* other_reactors, input_t<T> OtherReactorType::*other_member) {
        auto reactor_itr = reactors->begin();
        size_t left_ports = 0;
        size_t right_ports = other_reactors->size();

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_reactors->begin();

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_reactor_itr == other_reactors->end()) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                auto *right_reactor = (*right_reactor_itr).get();
                l_port.environment()->draw_connection(l_port, right_reactor->*other_member, reactor::ConnectionProperties{});
                ++right_reactor_itr;
                if (right_reactor_itr == other_reactors->end()) {
                    break;
                }
            }
        }
    }

    template <typename OtherReactorType>
    void operator>(std::pair<std::vector<std::unique_ptr<OtherReactorType>>*, input_t<T> OtherReactorType::*> connections)
    {
        this->connect_ports (connections.first, connections.second);
    }

    template <typename OtherReactorType>
    void connect_ports(std::vector<std::unique_ptr<OtherReactorType>>* other_reactors, multiport_input_t<T> OtherReactorType::*other_member) {
        auto reactor_itr = reactors->begin();
        size_t left_ports = 0;
        size_t right_ports = 0;

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();
            left_ports += (left_reactor->*member).get_nports();
        }

        for (auto& p_right_reactor : *other_reactors) {
            auto *right_reactor = p_right_reactor.get();
            right_ports += (right_reactor->*member).get_nports();
        }

        if (left_ports < right_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (left_ports > right_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_reactor_itr = other_reactors->begin();
        auto right_port_itr =  (*right_reactor_itr).get()->*other_member.begin();
        auto right_port_itr_end =  (*right_reactor_itr).get()->*other_member.end();
        size_t right_port_count = 0;

        for (auto& p_left_reactor : *reactors) {
            auto *left_reactor = p_left_reactor.get();

            if (right_port_count == right_ports) {
                break;
            }

            for (auto &l_port : left_reactor->*member) {
                l_port.environment()->draw_connection(l_port, *right_port_itr, reactor::ConnectionProperties{});
                ++right_port_count;
                if (right_port_count == right_ports) {
                    break;
                }
                ++right_port_itr;
                if (right_port_itr == right_port_itr_end) {
                    ++right_reactor_itr;
                    right_port_itr =  (*right_reactor_itr).get()->*other_member.begin();
                    right_port_itr_end =  (*right_reactor_itr).get()->*other_member.end();
                }
            }
        }
    }

    template <typename OtherReactorType>
    void operator>(std::pair<std::vector<std::unique_ptr<OtherReactorType>>*, multiport_input_t<T> OtherReactorType::*> connections)
    {
        this->connect_ports (connections.first, connections.second);
    }

private:
    std::vector<std::unique_ptr<ReactorType>>* reactors;
    multiport_output_t<T> ReactorType::*member;
};

// #define BANKED_OUTPUTS(vector, member) ({ \
//     if constexpr (is_specialization_v<std::decay_t<decltype(strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)>, output_t>) { \
//         BankedOutputConnector(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member); \
//     } else if constexpr (is_specialization_v<std::decay_t<decltype(strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)>, multiport_output_t>) { \
//         BankedMultiportOutputConnector(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member); \
//     } else { \
//         static_assert(false, "Unsupported member type for BankedConnector."); \
//     } \
// })
    // BankedOutputConnector(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)

#define BANKED_OUTPUTS(vector, member) \
    BankedOutputConnector(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)

#define BANKED_MP_OUTPUTS(vector, member) \
    BankedMultiportOutputConnector(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)

// template <typename VectorType, typename MemberType>
// auto BANKED_OUTPUTS(VectorType& vector, MemberType member) {
//     using BaseType = strip_smart_pointer_t<typename std::decay_t<VectorType>::value_type>;
//     using MemberTypeDecay = std::decay_t<decltype(((BaseType*)nullptr)->*member)>;

//     // static_assert(std::is_same_v<BaseType, MemberTypeDecay>,
//     //               "Expected MemberTypeDecay to be output_t<unsigned long long>");

//     // static_assert(std::is_same_v<typename MemberTypeDecay::value_type, unsigned long long>,
//     //               "Expected MemberTypeDecay to be output_t<unsigned long long>");

//     if constexpr (std::is_same_v<MemberTypeDecay, output_t<typename MemberTypeDecay::value_type>>) {
//         return BankedOutputConnector(&vector, member);
//     } else if constexpr (std::is_same_v<MemberTypeDecay, multiport_output_t<typename MemberTypeDecay::value_type>>) {
//         return BankedMultiportOutputConnector(&vector, member);
//     } else {
//         static_assert(!std::is_same_v<MemberTypeDecay, MemberTypeDecay>, "Unsupported member type for BankedConnector.");
//     }
// }


template <typename OutputTuple>
class MergedOutputsConnector
{
private:
    OutputTuple outputs;

public:
    MergedOutputsConnector(OutputTuple outputs)
    : outputs(std::move(outputs)) {}
    ~MergedOutputsConnector() {}

    template <typename Port>
    int count_ports(Port&& port) const {
        if constexpr (is_specialization_v<std::decay_t<Port>, multiport_output_t>) {
            return port->get_nports();
        } else {
            return 1;
        }
    }

    template <typename Port, typename RightItr>
    void connect_ports(Port&& port, RightItr& right_itr, RightItr right_end) const {
        if (right_itr == right_end) {
            return;
        }

        static_assert(  std::is_same_v<typename std::remove_pointer_t<std::decay_t<Port>>::value_type, typename std::decay_t<decltype(*right_itr)>::value_type>,
                        "Mismatched types: output_t<T> and input_t<T> must have the same T.");

        if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Port>>, multiport_output_t>) {
            for (auto& l_port : *port) {
                l_port.environment()->draw_connection(l_port, *right_itr, reactor::ConnectionProperties{});
                ++right_itr;
                if (right_itr == right_end) {
                    break;
                }
            }
        } else if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Port>>, output_t>) {
            port->environment()->draw_connection(*port, *right_itr, reactor::ConnectionProperties{});
            ++right_itr;
        } else {
            static_assert(false, "Unsupported output type");
        }
    }

    template <typename T>
    void operator>(multiport_input_t<T>& input) {
        int left_ports = 0;
        std::apply(
            [&](auto&&... left_port) {
                ((left_ports += count_ports (left_port)), ...);
            },
            outputs);

        if (input.get_nports() > left_ports) {
            reactor::log::Warn() << "There are more right ports than left ports. "
                                << "Not all ports will be connected!";
        } else if (input.get_nports() < left_ports) {
            reactor::log::Warn() << "There are more left ports than right ports. "
                                << "Not all ports will be connected!";
        }

        auto right_itr = input.begin();
        auto right_end = input.end();

        std::apply(
            [&](auto&&... left_port) {
                (connect_ports(left_port, right_itr, right_end), ...);
            },
            outputs);
    }
};

#define MERGED_OUTPUTS(...) \
    MergedOutputsConnector(std::make_tuple(__VA_ARGS__))

template <typename T>
struct trigger_value_type;

template <typename T>
struct trigger_value_type<reactor::LogicalAction<T> *>
{
    using type = reactor::LogicalAction<T>&;
};

template <typename T>
struct trigger_value_type<input_t<T> *>
{
    using type = input_t<T>&;
};

template <typename T>
struct trigger_value_type<multiport_input_t<T> *>
{
    using type = multiport_input_t<T>&;
};

template <typename T>
struct trigger_value_type<multiport_output_t<T> *>
{
    using type = multiport_output_t<T>&;
};

template <typename T>
struct trigger_value_type<output_t<T> *>
{
    using type = output_t<T>&;
};

template <>
struct trigger_value_type<reactor::StartupTrigger *>
{
    using type = reactor::StartupTrigger&;
};

template <>
struct trigger_value_type<reactor::ShutdownTrigger *>
{
    using type = reactor::ShutdownTrigger&;
};

template<typename T>
using logical_action_t = reactor::LogicalAction<T>;

using startup_t = reactor::StartupTrigger;
using shutdown_t = reactor::ShutdownTrigger;

class MagnitionTrigger : public std::enable_shared_from_this<MagnitionTrigger>
{
public:
    MagnitionReactor *reactor;
    std::shared_ptr<MagnitionTrigger> next;
    std::string name;

public:
    MagnitionTrigger(std::string name, MagnitionReactor *parent)
    : reactor(parent), next(nullptr), name (name) {}
    virtual ~MagnitionTrigger() = default;
};

template <typename InputTuple, typename OutputTuple>
class ReactionOutput: public MagnitionTrigger
{
private:
    InputTuple input_triggers;
    OutputTuple output_triggers;

public:
    explicit ReactionOutput(std::string name, MagnitionReactor *parent, InputTuple inputs, OutputTuple outputs)
        : MagnitionTrigger (name, parent), input_triggers(std::move(inputs)), output_triggers(std::move(outputs)) { /* std::cout << "Creating ReactionOutput\n"; */ }
    ~ReactionOutput() { /* std::cout << "Deleting ReactionOutput\n"; */ }

    template <typename Fn>
    void operator=(Fn func)
    {
        auto MagnitionReactionRef = std::make_shared<MagnitionReaction<Fn, InputTuple, OutputTuple>> (name, reactor, std::move(input_triggers), std::move(output_triggers), std::forward<Fn>(func));
        MagnitionReactionRef->execute();
    }

    template <typename Fn>
    void function(Fn func)
    {
        auto MagnitionReactionRef = std::make_shared<MagnitionReaction<Fn, InputTuple, OutputTuple>> (name, reactor, std::move(input_triggers), std::move(output_triggers), std::forward<Fn>(func));
        MagnitionReactionRef->execute();
    }
};

template <typename InputTuple>
class ReactionInput: public MagnitionTrigger
{
private:
    InputTuple input_triggers;

public:
    explicit ReactionInput(std::string name, MagnitionReactor *parent, InputTuple inputs)
        : MagnitionTrigger (name, parent), input_triggers(std::move(inputs)) { /* std::cout << "Creating ReactionInput\n"; */ }
    ~ReactionInput() { /* std::cout << "Deleting ReactionInput\n"; */ }

    template <typename... Outputs>
    ReactionOutput<InputTuple, std::tuple<Outputs...>> &operator>(std::tuple<Outputs...> &&outputs)
    {
        auto ReactionOutputRef = std::make_shared<ReactionOutput<InputTuple, std::tuple<Outputs...>>> (name, reactor, std::move(input_triggers), std::move(outputs));
        next = ReactionOutputRef;
        return *ReactionOutputRef;
    }

    template <typename... Outputs>
    ReactionOutput<InputTuple, std::tuple<Outputs...>> &outputs(Outputs&&... outputs)
    {
        auto output_tuple = std::make_tuple(outputs...);
        auto ReactionOutputRef = std::make_shared<ReactionOutput<InputTuple, std::tuple<Outputs...>>> (name, reactor, std::move(input_triggers), std::move(output_tuple));
        next = ReactionOutputRef;
        return *ReactionOutputRef;
    }
};

// class ConfigNode {
// public:
//     const YAML::Node *node;
//     unsigned int n_configs;
//     unsigned int index_factor;

//     ConfigNode(const YAML::Node* n) : node(n), n_configs(1), index_factor(1) {}
// };

// class MagntionConfig {
// public:
//     std::unordered_map<std::string, std::unique_ptr<ConfigNode>> nodes;
//     unsigned int total_configs = 1;

//     void display() {
//         std::cout << "Total configs:" << total_configs << std::endl;
//         for (const auto& kv : nodes) {
//             string key = kv.first;
//             std::cout << "key:" << key << " has configs:" << kv.second->n_configs << " index_factor:" << kv.second->index_factor << std::endl;

//             auto *node = kv.second->node;
//             std::cout << "DISP key:" << key << " from MAP ConfigNode:" << (void*)kv.second.get() << " node:" << (void*)node << endl;
//             if (node->IsMap()) {
//                 cout << "Node is a map\n";
//                 for (const auto& sub_kv : *node) {
//                     std::string sub_key = sub_kv.first.as<std::string>();
//                     cout << "sub_key:" << sub_key << endl;
//                 }
//             } else {
//                 cout << "Node is not a map\n";
//             }
//         }
//     }
// };

template <typename T>
struct ParameterMetadata {
    std::string name;
    std::string description;
    T min_value;
    T max_value;
    T value;
};

class MagnitionParameterBase {
public:
    virtual ~MagnitionParameterBase() = default;
    virtual void fetch_config() = 0;
    virtual void print() = 0;

    // virtual std::string to_json() const = 0;
    // virtual std::string to_yaml(std::string &&prefix) const = 0;

    friend class MagnitionReactor;
    friend class MagnitionSimulator;
};

template <typename T>
struct SystemParameterMetadata {
    std::vector<T> values;

    SystemParameterMetadata(std::initializer_list<T> val) : values(val) {}
};

class SystemParameterBase {
protected:
    virtual void pull_config() = 0;
    virtual void display() = 0;
    virtual int validate() = 0;

public:
    virtual ~SystemParameterBase() = default;
    virtual void pull_system_parameter(const std::string &key, void *user_param, const std::type_info& ti) = 0;

    template<typename T>
    void PullSystemParameter(const std::string &key, ParameterMetadata<T>* user_param) {
        pull_system_parameter(key, static_cast<void*>(user_param), typeid(T));
    }
    // virtual std::string to_json() const = 0;
    // virtual std::string to_yaml(std::string &&prefix) const = 0;
    friend class MagnitionSimulator;
};

template <typename... ParameterValueType>
class SystemParameter : public SystemParameterBase {
public:
    using ParameterValue = std::variant<SystemParameterMetadata<ParameterValueType>...>;
    using ParametersMap = std::map<std::string, ParameterValue>;

    virtual ParametersMap homogeneous_config() = 0;
    virtual ParametersMap heterogeneous_config() = 0;
    void pull_system_parameter(const std::string &key, void *user_param, const std::type_info& ti) override {
        auto itr_system = hetero_param_map.find(key);
        if (itr_system != hetero_param_map.end()) {
            auto v_it = hetero_invalid_keys.find(key);
            if (v_it != hetero_invalid_keys.end()) {
                hetero_invalid_keys.erase(v_it);
                // std::cout << "Key erased: " << key << std::endl;
            }
            std::visit([user_param, &ti, key](auto&& system_param) {
                using ContainerType = std::decay_t<decltype(system_param.values)>;
                using U = typename ContainerType::value_type;

                if (ti == typeid(U)) {
                    ParameterMetadata<U>* param = static_cast<ParameterMetadata<U>*>(user_param);
                    if ((system_param.values[0] < param->min_value) ||
                        (system_param.values[0] > param->max_value)) {
                        std::cout << "\nError: Range mismatch for parameter name: " << key << " value:" << system_param.values[0] <<
                            " min_value:" << param->min_value << " max_value:" << param->max_value << "\n";
                        std::exit(EXIT_FAILURE);
                    }
                    // std::cout << "Updating parameter name:" << key << " from value:" << param->value;
                    param->value = system_param.values[0];
                    // std::cout << " to value:" << param->value << std::endl;

                } else {
                    std::cout << "\nError: Type mismatch for parameter name: " << key << "\n"
                          << "Expected type: " << type_convert[ti.name()]
                          << ", Provided type: " << type_convert[typeid(U).name()] << std::endl;
                    std::exit(EXIT_FAILURE);
                }
            }, itr_system->second);
        } else {
            // std::cout << "Key:" << key << " not found in system parameters";
        }
    }

protected:
    std::map<std::string, ParameterValue> homoge_param_map;
    std::set<std::string> homoge_invalid_keys;
    std::map<std::string, ParameterValue> hetero_param_map;
    std::set<std::string> hetero_invalid_keys;
    void pull_config() override {
        homoge_param_map = homogeneous_config();
        for (const auto& entry : homoge_param_map) {
            // std::cout << "Validation Entry: " << entry.first << endl;
            bool result = homoge_invalid_keys.insert(entry.first).second;
            // cout << (result ? "UNIQUE\n" : "DUPICATE\n");
            assert(result);
        }

        hetero_param_map = heterogeneous_config();
        for (const auto& entry : hetero_param_map) {
            // std::cout << "Validation Entry: " << entry.first << endl;
            bool result = hetero_invalid_keys.insert(entry.first).second;
            // cout << (result ? "UNIQUE\n" : "DUPICATE\n");
            assert(result);
        }
    }

    int validate() override {
        for (const auto &key : hetero_invalid_keys) {
            cout << "Invalid key:" << key << "\n";
        }
        return hetero_invalid_keys.size();
    }

    void display() override {
        for (const auto& entry : hetero_param_map) {
            std::cout << "Parameter: " << entry.first << endl;

            std::visit([](auto&& param) {
                for (auto val : param.values) {
                    std::cout << "Value: " << val << std::endl;
                }
            }, entry.second);
        }
    }
};

class MagnitionSimulator: public reactor::Environment {
public:
    SystemParameterBase *system_parameters;
private:
    bool visualize = false;
    std::set<MagnitionReactor*> top_tier_reactors;
    // std::set<MagnitionParameterBase*> top_tier_params;
    static std::unique_ptr<MagnitionSimulator> instance;
    MagnitionSimulator(SystemParameterBase *sys_param, unsigned int num_workers = 1, bool fast_fwd_execution = true,
                       const reactor::Duration& timeout = reactor::Duration::max(), bool visualize = false);
    void print_child_parameters (MagnitionReactor *reactor);
public:
    // YAML::Node root;
    static MagnitionSimulator *create_simulator_instance(SystemParameterBase *sys_param = nullptr, unsigned int num_workers = 1, bool fast_fwd_execution = true,
                       const reactor::Duration& timeout = reactor::Duration::max(), bool visualize = false, string config_file = "");
    static MagnitionSimulator *get_simulator_instance();
    MagnitionSimulator(const MagnitionSimulator&) = delete;
    MagnitionSimulator& operator=(const MagnitionSimulator&) = delete;
    void run();
    void add_reactors (MagnitionReactor* reactor) {
        [[maybe_unused]] bool result = top_tier_reactors.insert(reactor).second;
        reactor_assert(result);
    }
    // void recurse_params_json (MagnitionParameterBase *param, std::ostringstream &oss);
    // void recurse_params_yaml (MagnitionParameterBase *param, std::ostringstream &oss, std::string &&prefix);
    // void add_params(MagnitionParameterBase *param) {
    //     [[maybe_unused]] bool result = top_tier_params.insert(param).second;
    //     reactor_assert(result);
    // }

    // std::unordered_map<std::string, std::string> magnition_config;
};

class MagnitionReactor : public reactor::Reactor
{
protected:
    reactor::StartupTrigger startup{"startup", this};
    reactor::ShutdownTrigger shutdown{"shutdown", this};

private:
    std::string current_reaction_name;
    std::unordered_map<std::string, std::shared_ptr<MagnitionTrigger>> reaction_map;
    int priority = 1;
    std::set<MagnitionReactor*> child_reactors;

    void add_child(MagnitionReactor* reactor) {
        [[maybe_unused]] bool result = child_reactors.insert(reactor).second;
        reactor_assert(result);
    }

    void add_to_reaction_map (std::string &name, std::shared_ptr<MagnitionTrigger> reaction) {
        reaction_map[name] = reaction;
    }

    int get_priority() { return priority++;}

    template <typename Fn, typename... InputTriggers, typename... OutputTriggers>
    void validate_reaction(Fn func, std::tuple<InputTriggers...> inputs, std::tuple<OutputTriggers...> outputs) {
        (void)func;
        (void)inputs;
        (void)outputs;
        static_assert(
            std::is_invocable_v<
                Fn,
                typename trigger_value_type<InputTriggers>::type...,
                typename trigger_value_type<OutputTriggers>::type...
                >,
                "Reaction function parameters must match the declared input and output types.");
    }

public:
    MagnitionParameterBase *p_param = nullptr;
    size_t bank_index = 0;
    MagnitionSimulator *sim;

    MagnitionReactor(const std::string &name, MagnitionSimulator *env)
        : reactor::Reactor(name, (reactor::Environment*)env), sim(env) {
        env->add_reactors (this);
        // reactor::validate(p_param,
        //             "Reactor:" + name + " has passed null parameter");
    }

    MagnitionReactor(const std::string &name, MagnitionReactor *container)
        : reactor::Reactor(name, container), sim(container->sim) {
        container->add_child (this);
        // reactor::validate(p_param,
        //     "Reactor:" + name + " has passed null parameter");
    }

    void set_param (MagnitionParameterBase *param) { p_param = param; }

    MagnitionReactor &reaction (const std::string name)
    {
        current_reaction_name = name;
        return *this;
    }

    template <typename... Inputs>
    ReactionInput<std::tuple<Inputs...>> &operator()(Inputs&&... inputs)
    {
        auto input_tuple = std::make_tuple(inputs...);
        auto ReactionInputRef = std::make_shared<ReactionInput<std::tuple<Inputs...>>> (current_reaction_name, this, std::move(input_tuple));
        reaction_map[current_reaction_name] = ReactionInputRef;
        return *ReactionInputRef;
    }

    template <typename... Inputs>
    ReactionInput<std::tuple<Inputs...>> &inputs(Inputs&&... inputs)
    {
        auto input_tuple = std::make_tuple(inputs...);
        auto ReactionInputRef = std::make_shared<ReactionInput<std::tuple<Inputs...>>> (current_reaction_name, this, std::move(input_tuple));
        reaction_map[current_reaction_name] = ReactionInputRef;
        return *ReactionInputRef;
    }

    template <typename Fn, typename... Inputs, typename... Outputs>
    void reaction(  const std::string &name,
                        std::tuple<Inputs...> &&inputs,
                        std::tuple<Outputs...> &&outputs,
                        Fn &&function)
    {
        auto MagnitionReactionRef =  std::make_shared<MagnitionReaction<Fn, std::tuple<Inputs...>, std::tuple<Outputs...>>>(name, this, std::move(inputs), std::move(outputs), std::forward<Fn>(function));
        MagnitionReactionRef->execute();
    }

    void request_stop() { environment()->sync_shutdown(); }
    virtual void construction() = 0;
    virtual void assembling() = 0;
    void construct() override {
        if (p_param) {
            p_param->fetch_config();
        }
        construction();
    }
    void assemble() override {
        assembling();
    }

    template <typename Fn, typename InputTuple, typename OutputTuple>
    friend class MagnitionReaction;

    friend class MagnitionSimulator;

    // template <typename... ParameterValueType>
    // friend class MagnitionParameter;
};

template <typename... ParameterValueType>
class MagnitionParameter : public MagnitionParameterBase {
public:
    using ParameterValue = std::variant<ParameterMetadata<ParameterValueType>*...>;

    MagnitionParameter(MagnitionReactor *owner)
    : reactor(owner), sim(owner->sim) {
        reactor->set_param (this);
    }

    void fetch_config() override {
        // cout << "Fetch config of Reactor:" << reactor->fqn() << endl;
        if (sim->system_parameters) {
            for (auto& entry : param_map) {
                // std::cout << "Fetching Parameter: " << entry.first << "\n";
                std::visit([&](auto* paramMetadataPtr) {
                    sim->system_parameters->PullSystemParameter(entry.first, paramMetadataPtr);
                }, entry.second);
            }
        }
    }

    void print() override {
        for (const auto& entry : param_map) {
            std::cout << "Parameter: " << entry.first << ", ";

            // Use std::visit to access the type inside the variant and print the description and default_value
            std::visit([](auto&& param) {
                int status;
                char* demangled_name = abi::__cxa_demangle(typeid(param->value).name(), nullptr, nullptr, &status);
                if (status == 0) {
                    std::cout << "Demangled type: " << demangled_name << std::endl;
                } else {
                    std::cout << "Error in demangling!" << std::endl;
                }
                std::free(demangled_name);
                std::cout << "Description: " << param->description 
                          << ", Value: " << param->value 
                          << ", Value Type Key: " << typeid(param->value).name()
                          << ", Value Type: " << type_convert [typeid(param->value).name()]
                          << std::endl;
            }, entry.second);
        }
    }

    template <typename... Args>
    void register_parameters(Args&... args) {
        register_parameters_(reactor->fqn(), args...);
    }

    // std::string to_json() const override {
    //     std::ostringstream oss;
    //     bool first = true;
    //     for (const auto& [key, meta] : param_map) {
    //         if (!first) oss << ",\n";
    //         first = false;

    //         oss << "  \"" << key << "\": {\n";
    //         oss << "    \"description\": \"" << meta.description << "\",\n";
    //         oss << "    \"value\": ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.getter());

    //         oss << ",\n    \"min\": ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.min_value);

    //         oss << ",\n    \"max\": ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.max_value);

    //         oss << "\n  }";
    //     }
    //     return oss.str();
    // }

    // std::string to_yaml(std::string &&prefix) const override {
    //     std::ostringstream oss;
    //     for (const auto& [key, meta] : param_map) {
    //         oss << prefix << key << ":\n"; // YAML key
    //         oss << prefix << "  description: \"" << meta.description << "\"\n";
    //         oss << prefix << "  value: ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.getter());
    //         oss << "\n";

    //         oss << prefix << "  min: ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.min_value);
    //         oss << "\n";

    //         oss << prefix << "  max: ";
    //         std::visit([&oss](auto&& val) {
    //             using T = std::decay_t<decltype(val)>;
    //             if constexpr (std::is_same_v<T, std::string>) {
    //                 oss << "\"" << val << "\"";
    //             } else {
    //                 oss << val;
    //             }
    //         }, meta.max_value);
    //         oss << "\n";
    //     }
    //     return oss.str();
    // }

private:
    std::map<std::string, ParameterValue> param_map;
    MagnitionReactor *reactor;
    MagnitionSimulator *sim;

    template <typename T>
    void register_parameter(const std::string& name, ParameterMetadata<T>& param) {
        param_map[name] = &param;
    }

    // Recursive function to handle inserting multiple parameters into the map
    template <typename T, typename... Args>
    void register_parameters_(const std::string& base_name, ParameterMetadata<T>& first, Args&... args) {
        register_parameter(base_name + "." + first.name, first);
        if constexpr (sizeof...(args) > 0) {
            register_parameters_(base_name, args...);
        }
    }

    friend class MagnitionReactor;
    friend class MagnitionSimulator;
};

template <typename Fn, typename InputTuple, typename OutputTuple>
class MagnitionReaction: public MagnitionTrigger
{
private:
    InputTuple input_triggers;
    OutputTuple output_triggers;
    Fn user_function;
    std::unique_ptr<reactor::Reaction> reaction;

    template <typename Reaction, typename Trigger>
    void set_input_trigger(Reaction &reaction, Trigger &&trigger)
    {
        if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, multiport_input_t>)
        {
            for (auto& port : *trigger) {
                reaction.declare_trigger(&port);
            }
        }
        else if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, multiport_output_t>)
        {
            for (auto& port : *trigger) {
                reaction.declare_trigger(&port);
            }
        }
        else {
            reaction.declare_trigger(trigger);
        }
    }

    template <typename Reaction, typename... Triggers>
    void set_input_triggers(std::unique_ptr<Reaction> &reaction, const std::tuple<Triggers...> &inputs)
    {
        std::apply([this, &reaction](auto &&...input)
            {
                (void)this;
                (..., set_input_trigger(*reaction, std::forward<decltype(input)>(input)));
            },
            inputs);
    }

    template <typename Reaction, typename Trigger>
    void set_output_trigger(Reaction &reaction, Trigger &&trigger)
    {
        if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, output_t>)
        {
            reaction.declare_antidependency(trigger);
        } else if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, input_t>)
        {
            reaction.declare_antidependency(trigger);
        }
        else if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, reactor::LogicalAction>)
        {
            reaction.declare_schedulable_action(trigger);
        }
        else if constexpr (is_specialization_v<std::remove_pointer_t<std::decay_t<Trigger>>, multiport_output_t>)
        {
            for (auto& port : *trigger) {
                reaction.declare_antidependency(&port);
            }
        }
        else
        {
            static_assert(false, "Unsupported trigger type");
        }
    }

    template <typename Reaction, typename... Triggers>
    void set_output_triggers(std::unique_ptr<Reaction> &reaction, const std::tuple<Triggers...> &outputs)
    {
        std::apply([this, &reaction](auto &&...output)
            {
                (void)this;
                (..., set_output_trigger(*reaction, std::forward<decltype(output)>(output)));
            },
            outputs);
    }

public:
    MagnitionReaction(std::string name, MagnitionReactor *parent, InputTuple inputs, OutputTuple outputs, Fn func)
        : MagnitionTrigger(name, parent), input_triggers(std::move(inputs)), output_triggers(std::move(outputs)),  user_function(std::forward<Fn>(func)) { /* std::cout << "Creating MagnitionReaction\n"; */ }
    ~MagnitionReaction() { /* std::cout << "Deleting MagnitionReaction\n"; */ }

    void execute () {
        int priority = reactor->get_priority();
        reactor->add_to_reaction_map(name, shared_from_this());
        reactor->validate_reaction (user_function, input_triggers, output_triggers);

        auto reactor_func = [func = std::move(user_function), this]()
        {
            (void)this;
            auto apply_to_dereferenced = [](auto&& func, auto&& tuple) {
                return std::apply(
                    [&](auto*... ptrs) {
                        return std::invoke(std::forward<decltype(func)>(func), (*ptrs)...);
                    },
                    std::forward<decltype(tuple)>(tuple));
            };

            apply_to_dereferenced(func, std::tuple_cat(this->input_triggers, this->output_triggers));
        };

        // std::cout << "Creating MagnitionReaction:" << name << " Priority:" << priority << "\n";
        reaction = std::make_unique<reactor::Reaction>(name, priority, reactor, reactor_func);

        set_input_triggers(reaction, input_triggers);
        set_output_triggers(reaction, output_triggers);
        
    }
};


// #define MAGNITION_REACTOR(name, members, constructor_args) \
// class name {                                               \
// public:                                                   \
//     members                                                \
// public:                                                    \
//     name constructor_args {}                               \
//                                                            \
//     void print() const {                                   \
//         std::cout << "Class " #name " created.\n";         \
//     }                                                      \
// };


// #define logical_action(T, name, dur) reactor::LogicalAction<T> name{#name, this, dur}
// #define input(T, name) reactor::Input<T> name{#name, this}
// #define output(T, name) reactor::Output<T> name{#name, this}



// #define MAGNITION_REACTOR(name)                                 \
// class name : public MagnitionReactor {                          \
// public:                                                         \
//     name(const std::string &name, reactor::Environment *env)    \
//         : MagnitionReactor(name, env, 0) {}                     \
//     name(const std::string &name, reactor::Reactor *container)  \
//         : MagnitionReactor(name, container, 0) {}               \
//                                                                 \
//     void print() const {                                        \
//         std::cout << "Class " #name " created.\n";              \
//     }

// #define MAGNITION_REACTIONS void construct() {

// #define MAGNITION_REACTIONS_END }

// #define MAGNITION_REACTOR_END(name) };
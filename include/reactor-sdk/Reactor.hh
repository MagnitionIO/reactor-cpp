#pragma once

#include "reactor-cpp/reactor-cpp.hh"
#include <string.h>
#include "BaseTrigger.hh"
#include "SystemParameterBase.hh"
#include "Environment.hh"

namespace sdk
{

template <typename InputTuple>
class ReactionInput;

template <typename Fn, typename InputTuple, typename OutputTuple>
class Reaction;

template<typename T>
class Input;

template<typename T>
class Output;

template<typename T>
class MultiportOutput;

template<typename T>
class MultiportInput;

template <typename T>
struct trigger_value_type;

template <typename T>
struct trigger_value_type<reactor::LogicalAction<T> *>
{
    using type = reactor::LogicalAction<T>&;
};

template <typename T>
struct trigger_value_type<Input<T> *>
{
    using type = Input<T>&;
};

template <typename T>
struct trigger_value_type<MultiportInput<T> *>
{
    using type = MultiportInput<T>&;
};

template <typename T>
struct trigger_value_type<MultiportOutput<T> *>
{
    using type = MultiportOutput<T>&;
};

template <typename T>
struct trigger_value_type<Output<T> *>
{
    using type = Output<T>&;
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

template <>
struct trigger_value_type<Timer *>
{
    using type = Timer&;
};

class Reactor : public reactor::Reactor
{
protected:
    reactor::StartupTrigger startup{"startup", this};
    reactor::ShutdownTrigger shutdown{"shutdown", this};

private:
    SystemParameterBase *p_param = nullptr;
    Environment *env;
    std::string current_reaction_name;
    std::unordered_map<std::string, std::shared_ptr<BaseTrigger>> reaction_map;
    int priority = 1;
    std::set<Reactor*> child_reactors;

    void add_child(Reactor* reactor) {
        [[maybe_unused]] bool result = child_reactors.insert(reactor).second;
        reactor_assert(result);
    }

    void add_to_reaction_map (std::string &name, std::shared_ptr<BaseTrigger> reaction) {
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
    size_t bank_index = 0;

    Reactor(const std::string &name, Environment *env)
        : reactor::Reactor(name, (reactor::Environment*)env), env(env) {
        // reactor::validate(p_param,
        //             "Reactor:" + name + " has passed null parameter");
    }

    Reactor(const std::string &name, Reactor *container)
        : reactor::Reactor(name, container), env(container->env) {
        container->add_child (this);
        // reactor::validate(p_param,
        //     "Reactor:" + name + " has passed null parameter");
    }

    void set_param (SystemParameterBase *param) { p_param = param; }

    Environment *get_env() { return env; }

    Reactor &reaction (const std::string name)
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
    ReactionInput<std::tuple<Inputs...>> &triggers(Inputs&&... inputs)
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
        auto ReactionRef =  std::make_shared<Reaction<Fn, std::tuple<Inputs...>, std::tuple<Outputs...>>>(name, this, std::move(inputs), std::move(outputs), std::forward<Fn>(function));
        ReactionRef->execute();
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
    friend class Reaction;
};

} // namespace sdk
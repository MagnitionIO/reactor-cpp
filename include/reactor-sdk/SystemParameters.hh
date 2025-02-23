#pragma once

#include <string>
#include <cxxabi.h>
#include "SystemParameterBase.hh"
#include "Environment.hh"

namespace sdk
{

template <typename T>
struct ParameterMetadata {
    std::string name;
    std::string description;
    T min_value;
    T max_value;
    T value;
};

template <typename... ParameterValueType>
class SystemParameter : public SystemParameterBase {
public:
    using ParameterValue = std::variant<ParameterMetadata<ParameterValueType>*...>;

    SystemParameter(Reactor *owner)
    : reactor(owner), env(owner->get_env()) {
        reactor->set_param (this);
    }

    void fetch_config() override {
        // cout << "Fetch config of Reactor:" << reactor->fqn() << endl;
        if (env->get_config_params()) {
            for (auto& entry : param_map) {
                // std::cout << "Fetching Parameter: " << entry.first << "\n";
                std::visit([&](auto* paramMetadataPtr) {
                    env->get_config_params()->PullConfigParameter(entry.first, paramMetadataPtr);
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

private:
    std::map<std::string, ParameterValue> param_map;
    Reactor *reactor;
    Environment *env;

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
};

} // namespace sdk
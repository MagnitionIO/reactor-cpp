#pragma once

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
    : reactor(owner), sim(owner->sim) {
        reactor->set_param (this);
    }

    void fetch_config() override {
        // cout << "Fetch config of Reactor:" << reactor->fqn() << endl;
        if (sim->config_parameters) {
            for (auto& entry : param_map) {
                // std::cout << "Fetching Parameter: " << entry.first << "\n";
                std::visit([&](auto* paramMetadataPtr) {
                    sim->config_parameters->PullConfigParameter(entry.first, paramMetadataPtr);
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
    Reactor *reactor;
    Environment *sim;

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

    friend class Reactor;
    friend class Environment;
};

} // namespace sdk
#pragma once

#include <map>
#include "ConfigParameterBase.hh"

namespace sdk
{

extern std::map<std::string, std::string> type_convert;

template <typename T>
struct ConfigParameterMetadata {
    std::vector<T> values;

    ConfigParameterMetadata(std::initializer_list<T> val) : values(val) {}
};

template <typename... ParameterValueType>
class ConfigParameter : public ConfigParameterBase {
public:
    using ParameterValue = std::variant<ConfigParameterMetadata<ParameterValueType>...>;
    using ParametersMap = std::map<std::string, ParameterValue>;

    virtual ParametersMap homogeneous_config() = 0;
    virtual ParametersMap heterogeneous_config() = 0;
    void pull_config_parameter(const std::string &key, void *user_param, const std::type_info& ti) override {
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
            std::cout << "Invalid key:" << key << "\n";
        }
        return hetero_invalid_keys.size();
    }

    void display() override {
        for (const auto& entry : hetero_param_map) {
            std::cout << "Parameter: " << entry.first << std::endl;

            std::visit([](auto&& param) {
                for (auto val : param.values) {
                    std::cout << "Value: " << val << std::endl;
                }
            }, entry.second);
        }
    }
};

} // namespace sdk
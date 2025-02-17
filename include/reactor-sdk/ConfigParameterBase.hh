#pragma once
#include <string.h>

namespace sdk
{

template <typename T>
struct ParameterMetadata;

class ConfigParameterBase {
protected:
    virtual void pull_config() = 0;
    virtual void display() = 0;
    virtual int validate() = 0;

public:
    virtual ~ConfigParameterBase() = default;
    virtual void pull_config_parameter(const std::string &key, void *user_param, const std::type_info& ti) = 0;

    template<typename T>
    void PullConfigParameter(const std::string &key, ParameterMetadata<T>* user_param) {
        pull_config_parameter(key, static_cast<void*>(user_param), typeid(T));
    }
    // virtual std::string to_json() const = 0;
    // virtual std::string to_yaml(std::string &&prefix) const = 0;
    friend class Environment;
};
    
} // namespace sdk
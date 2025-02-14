#ifndef USER_PARAMETERS_H
#define USER_PARAMETERS_H

#include <reactor-sdk/magnition-reactor-cpp.hh>
#include <map>
#include <variant>
#include <string>

struct UserParameters : public SystemParameter<int, uint32_t> {
    SystemParameter<int, uint32_t>::ParametersMap homogeneous_config();
    SystemParameter<int, uint32_t>::ParametersMap heterogeneous_config();
};

// using ParametersMap = std::map<std::string, SystemParameterMetadata<std::variant<SystemParameterMetadata<ParameterValueType>...>>>;

extern UserParameters system_parameters;

#endif // USER_PARAMETERS_H

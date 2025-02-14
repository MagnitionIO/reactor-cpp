#include "Config-a.hh"

UserParameters system_parameters;

SystemParameter<int, uint32_t>::ParametersMap UserParameters::homogeneous_config() {
    return {
            {"Main.Source.iterations", SystemParameterMetadata<int> { 0 } }
    };
}

SystemParameter<int, uint32_t>::ParametersMap UserParameters::heterogeneous_config() {
    return {
            {"Main.Source.iterations", SystemParameterMetadata<int> { 20 } },
            {"Main.Source.n_ports", SystemParameterMetadata<int> { 4 } },
            {"Main.n_sinks", SystemParameterMetadata<int> { 4 } },

    };
}

// UserParameters::filter_out () {
//     if (cfg_map["T0.P0.L1.n_ervers"] != cfg_map["T0.P0.L2.n_ervers"]) {

//     }
// }

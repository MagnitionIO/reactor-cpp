#include "Config-a.hh"

UserParameters system_parameters;

SystemParameter<int, reactor::Duration>::ParametersMap UserParameters::homogeneous_config() {
    return {
            {"Main.Source.iterations", SystemParameterMetadata<int> { 0 } }
    };
}

SystemParameter<int, reactor::Duration>::ParametersMap UserParameters::heterogeneous_config() {
    return {
            {"Main.slow.period", SystemParameterMetadata<reactor::Duration> { 1s } },
            {"Main.slow.duration", SystemParameterMetadata<reactor::Duration> { 5s } },
            {"Main.n_fast", SystemParameterMetadata<int> { 3 } },
            {"Main.fast_0.period", SystemParameterMetadata<reactor::Duration> { 500ms } },
            {"Main.fast_0.duration", SystemParameterMetadata<reactor::Duration> { 10ms } }
    };
}

// UserParameters::filter_out () {
//     if (cfg_map["T0.P0.L1.n_ervers"] != cfg_map["T0.P0.L2.n_ervers"]) {

//     }
// }

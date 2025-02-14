#pragma once

#include <reactor-sdk/magnition-reactor-cpp.hh>
using namespace std;

class SourceReactor : public MagnitionReactor {
public:
    struct Parameters : public MagnitionParameter<int> {

        ParameterMetadata<int> iterations = ParameterMetadata<int> {
            .name = "iterations",
            .description = "Number of iterations",
            .min_value = 1,
            .max_value = 100,
            .value = 10
        };

        ParameterMetadata<int> n_ports = ParameterMetadata<int> {
            .name = "n_ports",
            .description = "Size of multiports",
            .min_value = 1,
            .max_value = 10,
            .value = 1
        };

        Parameters(MagnitionReactor *container)
            :   MagnitionParameter<int>(container) {
            register_parameters (iterations, n_ports);
        }
    };
private:
    Parameters parameters{this};

    std::string name = "Source";
    int itr = 0;
    int rsp_itr = 0;
public:                                                         
    SourceReactor(const std::string &name, MagnitionSimulator *env)
        : MagnitionReactor(name, env) {}
    SourceReactor(const std::string &name, MagnitionReactor *container)
        : MagnitionReactor(name, container) {}

    logical_action_t<int> sch{"sch", this};
    multiport_input_t<int> rsp{"rsp", this};
    multiport_output_t<int> req{"req", this};

    void construction() override;
    void assembling() override;
};
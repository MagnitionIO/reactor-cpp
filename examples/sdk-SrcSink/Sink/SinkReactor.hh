#pragma once

#include <reactor-sdk/magnition-reactor-cpp.hh>
using namespace std;

class SinkReactor : public MagnitionReactor {
public:
    struct Parameters : public MagnitionParameter<string> {
        ParameterMetadata<string> name = ParameterMetadata<string> {
            .name = "Name",
            .description = "Alternate name",
            .min_value = "Sink",
            .max_value = "Sink",
            .value = "Sink"
        };

        Parameters(MagnitionReactor *container)
            :   MagnitionParameter<string>(container) {
            register_parameters (name);
        }
    };

    SinkReactor(const std::string &name, MagnitionSimulator *env)
        : MagnitionReactor(name, env) {}
    SinkReactor(const std::string &name, MagnitionReactor *container)
        : MagnitionReactor(name, container) {}

    Parameters parameters{this};

    input_t<int> req{"req", this};
    output_t<int> rsp{"rsp", this};
    
    void construction() override;
    void assembling() override;

    void startup_reaction (startup_t& startup);
    void process_request (input_t<int>& req, output_t<int>& rsp);
};

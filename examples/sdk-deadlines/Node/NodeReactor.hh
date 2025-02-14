#pragma once

#include <reactor-sdk/magnition-reactor-cpp.hh>

class NodeReactor: public MagnitionReactor {
public:
    struct Parameters : public MagnitionParameter<reactor::Duration> {
        ParameterMetadata<reactor::Duration> period = ParameterMetadata<reactor::Duration> {
            .name = "period",
            .description = "Schedule and deadline period",
            .min_value = 10ms,
            .max_value = 10s,
            .value = 500ms
        };

        ParameterMetadata<reactor::Duration> duration = ParameterMetadata<reactor::Duration> {
            .name = "duration",
            .description = "Sleep duration",
            .min_value = 5ms,
            .max_value = 5s,
            .value = 10ms
        };

        Parameters(MagnitionReactor *container)
            :   MagnitionParameter<reactor::Duration>(container) {
            register_parameters (period, duration);
        }
  };

private:
    Parameters parameters{this};

    logical_action_t<void> a{"a", this};

public:
    NodeReactor(const std::string &name, MagnitionSimulator *env)
    : MagnitionReactor(name, env) {}
    NodeReactor(const std::string &name, MagnitionReactor *container)
    : MagnitionReactor(name, container) {}
  
    void construction() override;
    void assembling() override;
};

        

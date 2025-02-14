#pragma once

#include <reactor-sdk/magnition-reactor-cpp.hh>

#include "Node/NodeReactor.hh"

class MainReactor: public MagnitionReactor {
public:
    struct Parameters : public MagnitionParameter<int> {
        ParameterMetadata<int> n_fast = ParameterMetadata<int> {
            .name = "n_fast",
            .description = "Number of fast nodes",
            .min_value = 1,
            .max_value = 10,
            .value = 2
        };

        Parameters(MagnitionReactor *container)
            :   MagnitionParameter<int>(container) {
            register_parameters (n_fast);
        }
    };
private:
    Parameters parameters{this};
    std::unique_ptr<NodeReactor> slow;
    ReactorBank<NodeReactor> fast;

public:
    MainReactor(const std::string &name, MagnitionSimulator *env)
    : MagnitionReactor(name, env) {}
    MainReactor(const std::string &name, MagnitionReactor *container)
    : MagnitionReactor(name, container) {}
  
    void construction() override;
    void assembling() override;
};

        

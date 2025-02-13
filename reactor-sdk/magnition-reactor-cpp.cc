#include <fstream>
#include <unordered_map>
#include "reactor-sdk/magnition-reactor-cpp.hh"

using namespace std;

std::unique_ptr<MagnitionSimulator> MagnitionSimulator::instance = nullptr;

std::map<std::string, std::string> type_convert = {
    {"i", "int"},
    {"j", "uint32_t"},
    {"b", "bool"},
    {"f", "float"},
    {"d", "double"},
    {"c", "char"},
    {"s", "std::string"},
    {"ll", "long long"},
    {"ul", "unsigned long"},
    {"uc", "unsigned char"},
    {"ld", "long double"},
    {"uint", "unsigned int"}
};

// void flattenYaml(const YAML::Node& node, const std::string& prefix, std::unordered_map<std::string, std::string>& flatMap) {
//     if (node.IsMap()) {
//         for (const auto& kv : node) {
//             flattenYaml(kv.second, prefix + kv.first.as<std::string>() + ".", flatMap);
//         }
//     } else if (node.IsSequence()) {
//         std::vector<std::string> values;
//         for (std::size_t i = 0; i < node.size(); ++i) {
//             values.push_back(node[i].as<std::string>());
//         }

//         std::string value_str = "[" + values[0];
//         for (std::size_t i = 1; i < values.size(); ++i) {
//             value_str += "," + values[i];
//         }
//         value_str += "]";

//         flatMap[prefix.substr(0, prefix.size() - 1)] = value_str; // Remove trailing dot
//     } else if (node.IsScalar()) {
//         flatMap[prefix.substr(0, prefix.size() - 1)] = node.as<std::string>(); // Remove trailing dot
//     } else {
//         std::cout << "Unsupported type\n";
//     }
// }

void generateDotFile(const std::unordered_map<std::string, std::vector<std::string>>& graph) {
    std::ofstream dotFile("graph.dot");
    dotFile << "Topology {" << std::endl;

    for (const auto& [source, destinations] : graph) {
        for (const auto& dest : destinations) {
            dotFile << "    " << source << " -> " << dest << ";" << std::endl;
        }
    }

    dotFile << "}" << std::endl;
    dotFile.close();
}

MagnitionSimulator::MagnitionSimulator( SystemParameterBase *sys_param, unsigned int num_workers, bool fast_fwd_execution,
                                        const reactor::Duration& timeout, bool visualize)
    : reactor::Environment (num_workers, fast_fwd_execution, timeout), system_parameters(sys_param), visualize(visualize)
{

}

MagnitionSimulator* MagnitionSimulator::create_simulator_instance   (SystemParameterBase *sys_param, unsigned int num_workers, bool fast_fwd_execution,
                                                                    const reactor::Duration& timeout, bool visualize, string config_file) {
    if (!instance) {
        instance = std::unique_ptr<MagnitionSimulator>(new MagnitionSimulator(sys_param, num_workers, fast_fwd_execution, timeout, visualize));

        // if (config_file != "") {
        //     std::cout << "LOADING CONFIG_FILE:" << config_file << std::endl;
        //     std::ifstream file(config_file);
        //     if (!file) {
        //         std::cerr << "Failed to open config.yaml" << std::endl;
        //     } else {
        //         instance->root = YAML::Load(file);
        //         // YAML::Node config = YAML::Load(file);
        //         flattenYaml(instance->root, "", instance->magnition_config);
        //         // instance->magnition_config.display();
        //         std::cout << "Flattened YAML Map:" << std::endl;
        //         for (const auto& kv : instance->magnition_config) {
        //             std::cout << kv.first << ": " << kv.second << std::endl;
        //         }
        //     }
        // }
    }
    return instance.get();
}

MagnitionSimulator* MagnitionSimulator::get_simulator_instance()
{
    if (!instance) {
        return nullptr;
    }
    return instance.get();
}

// void MagnitionSimulator::recurse_params_json (MagnitionParameterBase *param, std::ostringstream &oss) {
//     // bool first = true;
//     for (auto* child : param->child_params) {
//         // if (!first)
//             oss << ",\n";
//         // first = false;
//         oss << "\"" << child->fqname << "\": ";
//         oss << "{\n";
//         oss << child->to_json();
//         recurse_params_json (child, oss);
//         oss << "\n}\n";
//     }
// }

// void MagnitionSimulator::recurse_params_yaml (MagnitionParameterBase *param, std::ostringstream &oss, std::string &&prefix) {
//     for (auto* child : param->child_params) {
//         oss << prefix << child->fqname << ":\n";
//         std::string recurse_prefix = prefix + "  ";
//         std::string tmp_prefix = prefix + "  ";
//         oss << child->to_yaml(std::move(tmp_prefix));
//         recurse_params_yaml (child, oss, std::move(recurse_prefix));
//     }
// }

// void MagnitionSimulator::print_child_parameters (MagnitionReactor *reactor) {
//     for (auto *reactor_itr : reactor->child_reactors) {
//         std::cout << "Parent Reactor:" << reactor_itr->fqn() << " address:" << (void*)reactor_itr << "\n";
//         for (auto *input_itr : reactor_itr->inputs_) {
//             cout << "input:" << input_itr->fqn() << " address:" << (void*)input_itr << "\n";
//         }

//         for (auto *output_itr : reactor_itr->outputs_) {
//             cout << "output:" << output_itr->fqn() << " address:" << (void*)output_itr << "\n";
//         }

//         for (auto *action_itr : reactor_itr->actions_) {
//             cout << "action:" << action_itr->fqn() << " address:" << (void*)action_itr << "\n";
//         }

//         for (auto *reaction_itr : reactor_itr->reactions_) {
//             cout << "reaction:" << reaction_itr->fqn() << " address:" << (void*)reaction_itr << "\n";
//         }

//         for (auto *child_reactor_itr : reactor_itr->reactors_) {
//             cout << "child reactor:" << child_reactor_itr->fqn() << " address:" << (void*)child_reactor_itr << "\n";
//         }
//         // if (reactor_itr->p_param) {
//         //     reactor_itr->p_param->print();
//         // } else {
//         //     std::cout << "Reactor:" << reactor_itr->fqn() << " has no parameters\n";
//         // }
//         print_child_parameters(reactor_itr);
//     }
// }

void MagnitionSimulator::run()
{
    if (instance->system_parameters) {
        instance->system_parameters->pull_config();
        // instance->system_parameters->display();
    }

    instance->construct();
    // cout << "========== PRE ASSEMBLE ==========\n";
    // for (auto *reactor_itr : top_tier_reactors) {
    //     std::cout << "Parent Reactor:" << reactor_itr->fqn() << " address:" << (void*)reactor_itr << "\n";
    //     for (auto *input_itr : reactor_itr->inputs_) {
    //         cout << "input:" << input_itr->fqn() << " address:" << (void*)input_itr << "\n";
    //     }

    //     for (auto *output_itr : reactor_itr->outputs_) {
    //         cout << "output:" << output_itr->fqn() << " address:" << (void*)output_itr << "\n";
    //     }

    //     for (auto *action_itr : reactor_itr->actions_) {
    //         cout << "action:" << action_itr->fqn() << " address:" << (void*)action_itr << "\n";
    //     }

    //     for (auto *reaction_itr : reactor_itr->reactions_) {
    //         cout << "reaction:" << reaction_itr->fqn() << " address:" << (void*)reaction_itr << "\n";
    //     }

    //     for (auto *child_reactor_itr : reactor_itr->reactors_) {
    //         cout << "child reactor:" << child_reactor_itr->fqn() << " address:" << (void*)child_reactor_itr << "\n";
    //     }
    //     // if (reactor_itr->p_param) {
    //     //     reactor_itr->p_param->print();
    //     // } else {
    //     //     std::cout << "Reactor:" << reactor_itr->fqn() << " has no parameters\n";
    //     // }
    //     print_child_parameters (reactor_itr);
    // }

    instance->assemble();
    // cout << "========== POST ASSEMBLE ==========\n";
    // for (auto *reactor_itr : top_tier_reactors) {
    //     std::cout << "Parent Reactor:" << reactor_itr->fqn() << "\n";
    //     for (auto *input_itr : reactor_itr->inputs_) {
    //         cout << "input:" << input_itr->fqn() << " address:" << (void*)input_itr << "\n";
    //     }

    //     for (auto *output_itr : reactor_itr->outputs_) {
    //         cout << "output:" << output_itr->fqn() << " address:" << (void*)output_itr << "\n";
    //     }

    //     for (auto *action_itr : reactor_itr->actions_) {
    //         cout << "action:" << action_itr->fqn() << " address:" << (void*)action_itr << "\n";
    //     }

    //     for (auto *reaction_itr : reactor_itr->reactions_) {
    //         cout << "reaction:" << reaction_itr->fqn() << " address:" << (void*)reaction_itr << "\n";
    //     }

    //     for (auto *child_reactor_itr : reactor_itr->reactors_) {
    //         cout << "child reactor:" << child_reactor_itr->fqn() << " address:" << (void*)child_reactor_itr << "\n";
    //     }
    //     // if (reactor_itr->p_param) {
    //     //     reactor_itr->p_param->print();
    //     // } else {
    //     //     std::cout << "Reactor:" << reactor_itr->fqn() << " has no parameters\n";
    //     // }
    //     print_child_parameters (reactor_itr);
    // }

    if (instance->system_parameters) {
        if (instance->system_parameters->validate() != 0) {
            cout << "INVALID CONFIGURATION!\n";
            return;
        }
    }

    if (visualize) {
        auto graph = this->getGraph();
        auto edges = graph.get_edges();
        std::unordered_map<std::string, std::vector<std::string>> graph_map{};
        for (auto const& [source, sinks] : edges) {

        auto* source_port = source.first;
        auto properties = source.second;

        if (properties.type_ == reactor::ConnectionType::Normal) {
            for (auto* const destination_port : sinks) {
            std::cout << "from: " << source_port->fqn() << "(" << source_port << ")"
                        << " --> to: " << destination_port->fqn() << "(" << destination_port << ")" << std::endl;
            graph_map.try_emplace (source_port->fqn(), std::vector<std::string>{});
            graph_map[source_port->fqn()].push_back (destination_port->fqn());
            }
        }
        }
        generateDotFile (graph_map);

        // std::ostringstream json_config;
        // bool first = true;
        // json_config << "{\n";
        // for (auto*param : top_tier_params) {
        //     if (!first) json_config << ",\n";
        //     first = false;
        //     json_config << "\"" << param->fqname << "\": ";
        //     json_config << "{\n";
        //     json_config << param->to_json();
        //     recurse_params_json (param, json_config);
        //     json_config << "\n}\n";
        // }
        // json_config << "}\n";
        // std::cout << json_config.str();

        // std::ofstream json_file("config.json");
        // if (!json_file) {
        //     std::cerr << "Error: Unable to open file config.json for writing.\n";
        //     return;
        // }
        // json_file << json_config.str();
        // json_file.close();

        // std::ostringstream yaml_config;
        // for (auto*param : top_tier_params) {
        //     yaml_config << param->fqname << ":\n";
        //     yaml_config << param->to_yaml("  ");
        //     recurse_params_yaml (param, yaml_config, "  ");
        // }
        // std::cout << yaml_config.str();

        // std::ofstream yaml_file("config.yaml");
        // if (!yaml_file) {
        //     std::cerr << "Error: Unable to open file config.yaml for writing.\n";
        //     return;
        // }
        // yaml_file << yaml_config.str();
        // yaml_file.close();

        return;
    }
    auto thread = instance->startup();
    thread.join();
}
#pragma once

// #include <type_traits>
// #include <functional>
// #include <unordered_map>
// #include <string>
// #include <any>
// #include <cxxabi.h>
// #include <yaml-cpp/yaml.h>


using namespace std;

#include "reactor-cpp/reactor-cpp.hh"
#include "Misc.hh"
#include "ReactorBank.hh"
#include "MultiportInput.hh"
#include "MultiportOutput.hh"
#include "OutputPort.hh"
#include "InputPort.hh"
#include "Environment.hh"
#include "Reaction.hh"
#include "Reactor.hh"
#include "ConfigParameters.hh"
#include "SystemParameters.hh"

// namespace sdk
// {

// template <typename Fn, typename InputTuple, typename OutputTuple>
// class Reaction;

// class Reactor;

// template<typename T>
// class Input;

// template<typename T>
// class Output;

// template<class T>
// class MultiportOutput;

// template<class T>
// class MultiportInput;


// template <typename T>
// struct strip_smart_pointer {
//     using type = T;
// };

// template <typename T>
// struct strip_smart_pointer<std::unique_ptr<T>> {
//     using type = T;
// };

// template <typename T>
// using strip_smart_pointer_t = typename strip_smart_pointer<T>::type;

// #define OUTPUTS(...) std::make_tuple(__VA_ARGS__)
// #define INPUTS(...) std::make_tuple(__VA_ARGS__)
// #define BANKED_INPUTS(vector, member) \
//     std::make_pair(&vector, &strip_smart_pointer_t<typename std::decay_t<decltype(vector)>::value_type>::member)





// // class ConfigNode {
// // public:
// //     const YAML::Node *node;
// //     unsigned int n_configs;
// //     unsigned int index_factor;

// //     ConfigNode(const YAML::Node* n) : node(n), n_configs(1), index_factor(1) {}
// // };

// // class MagntionConfig {
// // public:
// //     std::unordered_map<std::string, std::unique_ptr<ConfigNode>> nodes;
// //     unsigned int total_configs = 1;

// //     void display() {
// //         std::cout << "Total configs:" << total_configs << std::endl;
// //         for (const auto& kv : nodes) {
// //             string key = kv.first;
// //             std::cout << "key:" << key << " has configs:" << kv.second->n_configs << " index_factor:" << kv.second->index_factor << std::endl;

// //             auto *node = kv.second->node;
// //             std::cout << "DISP key:" << key << " from MAP ConfigNode:" << (void*)kv.second.get() << " node:" << (void*)node << endl;
// //             if (node->IsMap()) {
// //                 cout << "Node is a map\n";
// //                 for (const auto& sub_kv : *node) {
// //                     std::string sub_key = sub_kv.first.as<std::string>();
// //                     cout << "sub_key:" << sub_key << endl;
// //                 }
// //             } else {
// //                 cout << "Node is not a map\n";
// //             }
// //         }
// //     }
// // };


    
// } // namespace sdk



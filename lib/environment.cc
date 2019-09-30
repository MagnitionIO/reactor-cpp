/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Christian Menard
 */

#include "dear/environment.hh"
#include "dear/port.hh"
#include "dear/reaction.hh"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <map>

// xxx
#include <iostream>

namespace dear {

void Environment::register_reactor(Reactor* reactor) {
  assert(this->phase() == Phase::Construction);
  assert(reactor != nullptr);
  assert(reactor->is_top_level());
  auto r = _top_level_reactors.insert(reactor);
  assert(r.second);
}

void recursive_assemble(Reactor* container) {
  container->assemble();
  for (auto r : container->reactors()) {
    recursive_assemble(r);
  }
}

void Environment::assemble() {
  assert(_phase == Phase::Construction);
  _phase = Phase::Assembly;
  for (auto r : _top_level_reactors) {
    recursive_assemble(r);
  }
}

void Environment::build_dependency_graph(Reactor* reactor) {
  // obtain dependencies from each contained reactor
  for (auto r : reactor->reactors()) {
    build_dependency_graph(r);
  }
  // get reactions from this reactor; also order reactions by their priority
  std::map<int, Reaction*> priority_map;
  for (auto r : reactor->reactions()) {
    reactions.insert(r);
    auto result = priority_map.emplace(r->priority(), r);
    assert(result.second && "priorities must be unique (for now)");
  }

  // connect all reactions this reaction depends on
  for (auto r : reactor->reactions()) {
    for (auto d : r->dependencies()) {
      auto source = d;
      while (source->has_inward_binding()) {
        source = source->inward_binding();
      }
      for (auto ad : source->antidependencies()) {
        std::cout << r->fqn() << " depends on " << ad->fqn() << std::endl;
        dependencies.push_back(std::make_pair(r, ad));
      }
    }
  }

  // connect reactions by priority
  auto it = priority_map.begin();
  auto next = std::next(it);
  while (next != priority_map.end()) {
    dependencies.push_back(std::make_pair(next->second, it->second));
    it++;
    next = std::next(it);
  }
}

void Environment::init() {
  // build the dependency graph
  for (auto r : _top_level_reactors) {
    build_dependency_graph(r);
  }
}

std::string dot_name(ReactorElement* r) {
  std::string fqn = r->fqn();
  std::replace(fqn.begin(), fqn.end(), '.', '_');
  return fqn;
}

void Environment::export_dependency_graph(const std::string& path) {
  std::ofstream dot;
  dot.open(path);

  dot << "digraph {\n";
  for (auto r : reactions) {
    dot << dot_name(r) << " [label=\"" << r->fqn() << "\"];" << std::endl;
  }
  for (auto d : dependencies) {
    dot << dot_name(d.first) << " -> " << dot_name(d.second) << ';'
        << std::endl;
  }
  dot << "}\n";

  dot.close();
}

}  // namespace dear

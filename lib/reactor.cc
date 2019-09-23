/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Christian Menard
 */

#include "dear/reactor.hh"

#include <cassert>

namespace dear {

ReactorElement::ReactorElement(const std::string& name,
                               ReactorElement::Type type,
                               Reactor* container)
    : _name(name), _container(container) {
  assert(container != nullptr || type == ReactorElement::Type::Reactor);
  if (container != nullptr) {
    // We need a reinterpret_cast here as the derived class is not yet created
    // when this constructor is executed. dynamic_cast only works for
    // completely constructed objects. Technically, the casts here return
    // invalid pointers as the objects they point to do not yet
    // exists. However, we are good as long as we only store the pointer and do
    // not dereference it before construction completeted.
    // It works, but maybe there is some nicer way of doing this...
    switch (type) {
      case Type::Action:
        container->register_action(reinterpret_cast<BaseAction*>(this));
        break;
      case Type::Input:
        container->register_input(reinterpret_cast<BaseInput*>(this));
        break;
      case Type::Output:
        container->register_output(reinterpret_cast<BaseOutput*>(this));
        break;
      case Type::Reaction:
        container->register_reaction(reinterpret_cast<Reaction*>(this));
        break;
      case Type::Reactor:
        container->register_reactor(reinterpret_cast<Reactor*>(this));
        break;
      default:
        assert(false);
    }
  }
}

std::stringstream& ReactorElement::fqn_detail(std::stringstream& ss) const {
  if (this->is_top_level())
    ss << this->_name;
  else
    this->_container->fqn_detail(ss) << '.' << this->_name;
  return ss;
}

std::string ReactorElement::fqn() const {
  std::stringstream ss;
  return this->fqn_detail(ss).str();
}

void Reactor::register_action(BaseAction* action) {
  assert(action != nullptr);
  auto result = _actions.insert(action);
  assert(result.second);
}
void Reactor::register_input(BaseInput* input) {
  assert(input != nullptr);
  auto result = _inputs.insert(input);
  assert(result.second);
}
void Reactor::register_output(BaseOutput* output) {
  assert(output != nullptr);
  auto result = _outputs.insert(output);
  assert(result.second);
}
void Reactor::register_reaction(Reaction* reaction) {
  assert(reaction != nullptr);
  auto result = _reactions.insert(reaction);
  assert(result.second);
}
void Reactor::register_reactor(Reactor* reactor) {
  assert(reactor != nullptr);
  auto result = _reactors.insert(reactor);
  assert(result.second);
}

}  // namespace dear

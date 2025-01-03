/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Tassilo Tanneberger
 *   Christian Menard
 */

#include "reactor-cpp/reactor.hh"

#include "reactor-cpp/action.hh"
#include "reactor-cpp/assert.hh"
#include "reactor-cpp/environment.hh"
#include "reactor-cpp/port.hh"
#include "reactor-cpp/reaction.hh"
#include "reactor-cpp/statistics.hh"

namespace reactor {

ReactorElement::ReactorElement(const std::string& name, ReactorElement::Type type, Reactor* container)
    : name_(name)
    , container_(container) {
  reactor_assert(container != nullptr);
  this->environment_ = container->environment();
  reactor_assert(this->environment_ != nullptr);
//   validate(this->environment_->phase() == Phase::Construction ||
//                (type == Type::Action && this->environment_->phase() == Phase::Assembly),
//            "Reactor elements can only be created during construction phase!");
  // We need a reinterpret_cast here as the derived class is not yet created
  // when this constructor is executed. dynamic_cast only works for
  // completely constructed objects. Technically, the casts here return
  // invalid pointers as the objects they point to do not yet
  // exists. However, we are good as long as we only store the pointer and do
  // not dereference it before construction is completed.
  // It works, but maybe there is some nicer way of doing this...
  switch (type) {
  case Type::Action:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    container->register_action(reinterpret_cast<BaseAction*>(this));
    break;
  case Type::Input:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    container->register_input(reinterpret_cast<BasePort*>(this));
    break;
  case Type::Output:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    container->register_output(reinterpret_cast<BasePort*>(this));
    break;
  case Type::Reaction:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    container->register_reaction(reinterpret_cast<Reaction*>(this));
    break;
  case Type::Reactor:
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    container->register_reactor(reinterpret_cast<Reactor*>(this));
    break;
  default:
    throw std::runtime_error("unexpected type");
  }

  std::stringstream string_stream;
  string_stream << container_->fqn() << '.' << name;
  fqn_ = string_stream.str();
}

ReactorElement::ReactorElement(const std::string& name, ReactorElement::Type type, Environment* environment)
    : name_(name)
    , fqn_(name)
    , environment_(environment) {
  reactor_assert(environment != nullptr);

  validate(type == Type::Reactor || type == Type::Action, "Only reactors and actions can be owned by the environment!");
//   validate(this->environment_->phase() == Phase::Construction ||
//                (type == Type::Action && this->environment_->phase() == Phase::Assembly),
//            "Reactor elements can only be created during construction phase!");

  switch (type) {
  case Type::Action:
    Statistics::increment_actions();
    break;
  case Type::Reactor:
    Statistics::increment_reactor_instances();
    break;
  default:
    break;
  }
}
} // namespace reactor

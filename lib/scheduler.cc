/*
 * Copyright (C) 2019 TU Dresden
 * All rights reserved.
 *
 * Authors:
 *   Christian Menard
 */

#include "reactor-cpp/scheduler.hh"

#include "reactor-cpp/action.hh"
#include "reactor-cpp/assert.hh"
#include "reactor-cpp/environment.hh"
#include "reactor-cpp/logging.hh"
#include "reactor-cpp/port.hh"
#include "reactor-cpp/reaction.hh"
#include "reactor-cpp/trace.hh"

namespace reactor {

void Scheduler::work(unsigned id) {
  log::Debug() << "(Worker " << id << ") "
               << "Starting";

  while (true) {
    // wait for work
    sem_running_workers.acquire();
    log::Debug() << "(Worker " << id << ") "
                 << "Waking up";

    // break out of the loop if a termination was requested
    if (terminate_workers.load(std::memory_order_acquire))
      break;

    // process ready reactions as long as there are any
    process_ready_reactions(id);

    // Reaching this point means that all ready reactions are processed. In
    // consequence, all workers except one go to sleep and wait for ready
    // reactions.  The remaining worker takes care of the scheduling.
    if (using_workers) {
      if (running_workers.fetch_sub(1, std::memory_order_acq_rel) > 1) {
        log::Debug() << "(Worker " << id << ") "
                     << "Waiting for work";
        continue;  // go back to the top of the loop and wait for work
      }
      log::Debug() << "(Worker " << id << ") "
                   << "I am the last active worker.";
    }

    // Reaching this point means that the current worker thread is the last
    // running worker. Since the semaphore sem_running_workers guarantees
    // that no other worker is running, we can safely work with all the data
    // structures without acquiring additional mutexes.

    schedule_ready_reactions(id);

    unsigned num_ready_reactions = ready_reactions.size();

    if (num_ready_reactions == 0) {
      // if we reach this point, all reactions in the reaction queue for the
      // current tag where processed and we need to call next() or terminate the
      // execution
      if (continue_execution) {
        log::Debug() << "(Worker " << id << ") "
                     << "call next()";
        next();
        reaction_queue_pos = 0;

        schedule_ready_reactions(id);
        num_ready_reactions = ready_reactions.size();
      }

      if (!continue_execution && num_ready_reactions == 0) {
        // let all workers know that they should terminate
        terminate_workers.store(true, std::memory_order_release);
        running_workers.fetch_add(_environment->num_workers(),
                                  std::memory_order_acq_rel);
        sem_running_workers.release(_environment->num_workers());
        continue;
      }
    }

    // we use as many workers as there are ready reactions, but at most the
    // total number of workers we have
    unsigned workers_to_wakeup =
        std::min(num_ready_reactions, _environment->num_workers());
    log::Debug() << "(Worker " << id << ") wakeup " << workers_to_wakeup
                 << " workers";
    running_workers.fetch_add(workers_to_wakeup, std::memory_order_acq_rel);
    sem_running_workers.release(workers_to_wakeup);
  }

  log::Debug() << "Stopping worker " << id;
}

void Scheduler::process_ready_reactions(unsigned id) {
  // process ready reactions as long as there are any
  while (true) {
    // get the position of the next reaction to process via atomic decrement
    int pos = num_ready_reactions.fetch_sub(1, std::memory_order_acq_rel) - 1;

    // break out of the loop if we reached the end of the queue
    if (pos < 0)
      break;

    // read-only access to the ready_reactions vector is thread-safe
    auto reaction = ready_reactions[pos];

    // execute the reaction
    log::Debug() << "(Worker " << id << ") "
                 << "execute reaction " << reaction->fqn();
    tracepoint(reactor_cpp, reaction_execution_starts, id, reaction->fqn());
    reaction->trigger();
    tracepoint(reactor_cpp, reaction_execution_finishes, id, reaction->fqn());
  }
}

void Scheduler::schedule_ready_reactions(unsigned id) {
  // clear any old reactions that where already processed
  ready_reactions.clear();

  // Have we finished iterating over the reaction queue?
  if (reaction_queue_pos < reaction_queue.size()) {
    // No -> continue iterating
    log::Debug() << "(Worker " << id << ") "
                 << "Scanning the reaction queue for ready reactions";

    // continue the actual iteration
    while (reaction_queue_pos < reaction_queue.size() &&
           reaction_queue[reaction_queue_pos].empty()) {
      reaction_queue_pos++;
    }

    if (reaction_queue_pos < reaction_queue.size()) {
      auto& reactions = reaction_queue[reaction_queue_pos];

      // any ready reactions of current priority?
      if (!reactions.empty()) {
        log::Debug() << "(Worker " << id << ") "
                     << "Process reactions of priority " << reaction_queue_pos;

        // Make sure that any reaction is only executed once even if it
        // was triggered multiple times.
        std::sort(reactions.begin(), reactions.end());
        reactions.erase(std::unique(reactions.begin(), reactions.end()),
                        reactions.end());

        // place all ready reactions on the ready queue
        for (auto r : reactions) {
          log::Debug() << "(Worker " << id << ") "
                       << "Reaction " << r->fqn() << " is ready for execution";
          tracepoint(reactor_cpp, trigger_reaction, r->container()->fqn(),
                     r->name(), _logical_time);
          ready_reactions.push_back(r);
        }

        num_ready_reactions.store(reactions.size(), std::memory_order_release);
        reactions.clear();
      }
    } else {
      log::Debug() << "(Worker " << id << ") Reached end of reaction queue";
    }
  }
}

void Scheduler::start() {
  log::Debug() << "Starting the scheduler...";

  // initialize the reaction queue
  reaction_queue.resize(_environment->max_reaction_index() + 1);

  // start worker threads
  for (unsigned i = 1; i < _environment->num_workers() + 1; i++) {
    worker_threads.emplace_back([this, i]() { this->work(i); });
  }

  // join all worker threads
  for (auto& t : worker_threads) {
    t.join();
  }
}

void Scheduler::next() {
  static std::unique_ptr<EventMap> events{nullptr};

  // clean up before scheduling any new events
  if (events != nullptr) {
    // cleanup all triggered actions
    for (auto& kv : *events) {
      kv.first->cleanup();
    }
    events = nullptr;

    // cleanup all set ports
    for (auto p : set_ports) {
      p->cleanup();
    }
    set_ports.clear();
  }

  {
    std::unique_lock<std::mutex> lock{m_schedule};

    // shutdown if there are no more events in the queue
    if (event_queue.empty() && !_stop) {
      if (_environment->run_forever()) {
        // wait for a new asynchronous event
        cv_schedule.wait(lock,
                         [this]() { return !event_queue.empty() || _stop; });
      } else {
        log::Debug() << "No more events in queue. -> Terminate!";
        _environment->sync_shutdown();
      }
    }

    while (events == nullptr) {
      if (_stop) {
        continue_execution = false;
        log::Debug() << "Shutting down the scheduler";
        Tag t_next = Tag::from_logical_time(_logical_time).delay();
        if (t_next == event_queue.begin()->first) {
          log::Debug() << "Schedule the last round of reactions including all "
                          "termination reactions";
          events = std::move(event_queue.begin()->second);
          event_queue.erase(event_queue.begin());
          log::Debug() << "advance logical time to tag [" << t_next.time_point()
                       << ", " << t_next.micro_step() << "]";
          _logical_time.advance_to(t_next);
        } else {
          return;
        }
      } else {
        // collect events of the next tag
        auto t_next = event_queue.begin()->first;

        // synchronize with physical time if not in fast forward mode
        if (!_environment->fast_fwd_execution()) {
          // keep track of the current physical time in a static variable
          static auto physical_time = TimePoint::min();

          // If physical time is smaller than the next logical time point,
          // then update the physical time. This step is small optimization to
          // avoid calling get_physical_time() in every iteration as this
          // would add a significant overhead.
          if (physical_time < t_next.time_point())
            physical_time = get_physical_time();

          // If physical time is still smaller than the next logical time
          // point, then wait until the next tag or until a new event is
          // inserted asynchronously into the queue
          if (physical_time < t_next.time_point()) {
            auto status = cv_schedule.wait_until(lock, t_next.time_point());
            // Start over if the event queue was modified
            if (status == std::cv_status::no_timeout) {
              continue;
            } else {
              // update physical time and continue otherwise
              physical_time = t_next.time_point();
            }
          }
        }

        // retrieve all events with tag equal to current logical time from the
        // queue
        events = std::move(event_queue.begin()->second);
        event_queue.erase(event_queue.begin());

        // advance logical time
        log::Debug() << "advance logical time to tag [" << t_next.time_point()
                     << ", " << t_next.micro_step() << "]";
        _logical_time.advance_to(t_next);
      }
    }
  }  // mutex m_schedule

  // execute all setup functions; this sets the values of the corresponding
  // actions
  for (auto& kv : *events) {
    auto& setup = kv.second;
    if (setup != nullptr) {
      setup();
    }
  }

  for (auto& kv : *events) {
    for (auto n : kv.first->triggers()) {
      // There is no need to acquire the mutex. At this point the scheduler
      // should be the only thread accessing the reaction queue as none of the
      // workers are running
      reaction_queue[n->index()].push_back(n);
    }
  }
}

Scheduler::Scheduler(Environment* env)
    : using_workers(env->num_workers() > 1), _environment(env) {}

Scheduler::~Scheduler() {}

void Scheduler::schedule_sync(const Tag& tag,
                              BaseAction* action,
                              std::function<void(void)> setup) {
  ASSERT(_logical_time < tag);
  // TODO verify that the action is indeed allowed to be scheduled by the
  // current reaction
  log::Debug() << "Schedule action " << action->fqn()
               << (action->is_logical() ? " synchronously "
                                        : " asynchronously ")
               << " with tag [" << tag.time_point() << ", " << tag.micro_step()
               << "]";
  {
    auto lg = using_workers ? std::unique_lock<std::mutex>(m_event_queue)
                            : std::unique_lock<std::mutex>();

    tracepoint(reactor_cpp, schedule_action, action->container()->fqn(),
               action->name(), tag);

    // create a new event map or retrieve the existing one
    auto emplace_result =
        event_queue.try_emplace(tag, std::make_unique<EventMap>());
    auto& event_map = *emplace_result.first->second;

    // insert the new event
    event_map[action] = setup;
  }
}

void Scheduler::schedule_async(const Tag& tag,
                               BaseAction* action,
                               std::function<void(void)> setup) {
  std::lock_guard<std::mutex> lg(m_schedule);
  schedule_sync(tag, action, setup);
  cv_schedule.notify_one();
}

void Scheduler::set_port(BasePort* p) {
  log::Debug() << "Set port " << p->fqn();
  auto lg = using_workers ? std::unique_lock<std::mutex>(m_reaction_queue)
                          : std::unique_lock<std::mutex>();
  // We do not check here if p is already in the list. This means clean()
  // could be called multiple times for a single port. However, calling
  // clean() multiple time is not harmful and more efficient then checking if
  // the port is already in the list.
  set_ports.push_back(p);
  // recursively search for triggered reactions
  set_port_helper(p);
}

void Scheduler::set_port_helper(BasePort* p) {
  ASSERT(!(p->has_outward_bindings() && !p->triggers().empty()));
  if (p->has_outward_bindings()) {
    for (auto binding : p->outward_bindings()) {
      set_port_helper(binding);
    }
  } else {
    for (auto n : p->triggers()) {
      reaction_queue[n->index()].push_back(n);
    }
  }
}

void Scheduler::stop() {
  _stop = true;
  cv_schedule.notify_one();
}

}  // namespace reactor

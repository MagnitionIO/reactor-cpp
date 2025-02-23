
#include <reactor-sdk/reactor-sdk.hh>

using namespace std;
using namespace sdk;

class ActionValues : public Reactor {
    LogicalAction<int> act{"act", this};
    bool success = false;
    Duration zero = 0ns;
public:
    ActionIsPresent(const std::string &name, Environment *env)
        : Reactor(name, env) {}
    ActionIsPresent(const std::string &name, Reactor *container)
        : Reactor(name, container) {}
    
    void construction() {
    }

    void assembling() {
        reaction ("reaction_1").
            triggers(&startup, &a).
            effects().
            function (
                [&](Startup &startup, LogicalAction<void> &a) {
                    if (!a.is_present()) {
                        if (parameters.offset.value == zero) {
                            std::cout << "Hello World!" << '\n';
                            success = true;
                        } else {
                            a.schedule(parameters.offset.value);
                        }
                    } else {
                        std::cout << "Hello World 2!" << '\n';
                        success = true;
                    }
                }
            );
        
        reaction ("reaction_2").
            triggers(&shutdown).
            effects().
            function (
                [&](Shutdown &shutdown) {
                    if (!success) {
                        std::cerr << "Failed to print 'Hello World!'" << '\n';
                        exit(1);
                    }
                }
            );
    }
};

int main(int argc, char **argv) {
    unsigned workers = 1;
    bool fast{false};
    reactor::Duration timeout = reactor::Duration::max();

    bool visualize = false;

    if (argc > 1) {
        string v_str = argv[1];
        visualize =  (v_str == "true") ? true : false;
    }

    std::cout << "parameters - workers:" << workers << " fast:" << (fast ? "True" : "False") << " timeout:" << timeout << " visualize:" << visualize << std::endl;

    Environment sim {nullptr, workers, fast, timeout, visualize};
    auto action_delay = new ActionIsPresent("ActionIsPresent", &sim);

    sim.run();
    return 0;
}

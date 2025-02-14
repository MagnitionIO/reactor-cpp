
#include <reactor-sdk/magnition-reactor-cpp.hh>

#include "Config-a/Config-a.hh"
#include "Main/MainReactor.hh"

using namespace std;

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

    MagnitionSimulator sim {&system_parameters, workers, fast, timeout, visualize};
    auto main = new MainReactor("Main", &sim);

    sim.run();
    return 0;
}

#include "MainReactor.hh"

void MainReactor::construction() {

    cout << "Construction Main n_sinks:" << parameters.n_sinks.value << "\n";

    src = std::make_unique<SourceReactor>("Source", this);

    for (int __lf_idx = 0; __lf_idx < parameters.n_sinks.value; __lf_idx++) {
        std::string __lf_inst_name = "Sink_" + std::to_string(__lf_idx);
        snk.emplace_back(std::make_unique<SinkReactor>(__lf_inst_name, this));
    }
}

void MainReactor::assembling() {
    cout << "Assembling Main n_sinks:" << parameters.n_sinks.value << "\n";

    src->req --> snk->*(select_default(snk).req);
    (snk->*(select_default(snk).rsp)) --> src->rsp;

    reaction("reaction_1").
        triggers(&startup).
        effects().
        function(
            [&](Startup& startup) {
                cout << "(" << get_elapsed_logical_time() << ", " << get_microstep() << "), physical_time: " << get_elapsed_physical_time() << " " <<
                "Starting up reaction\n" << "Bank:" << bank_index << " name:" << parameters.alias.value << " fqn:" << fqn() << " n_sinks:" << parameters.n_sinks.value << endl;
            }
    );

    reaction("reaction_2").
        triggers(&snk[0].rsp).
        effects().
        function(
            [&](Output<int>& rsp) {
                cout << "(" << get_elapsed_logical_time() << ", " << get_microstep() << "), physical_time: " << get_elapsed_physical_time() << " " <<
                "Response\n" << "Bank:" << bank_index << " value:" << *rsp.get() << endl;
            }
    );
}
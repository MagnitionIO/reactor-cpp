#pragma once

#include "SinkReactor.hh"
using namespace std;
    
void SinkReactor::construction() {
    cout << "Construction Sink\n";
}

void SinkReactor::assembling() {

    cout << "Assembling Sink\n";

    reaction("startup_reaction").
        inputs(&startup).
        outputs().
        function(pass_function(startup_reaction)
    );

    reaction("process_request").
        inputs(&req).
        outputs(&rsp).
        function(pass_function(process_request)
    );
}



void SinkReactor::startup_reaction (startup_t& startup) {
    cout << "(" << get_elapsed_logical_time() << ", " << get_microstep() << "), physical_time: " << get_elapsed_physical_time() << " " <<
    "Starting up reaction\n" << "Bank:" << bank_index << " name:" << parameters.name.value << " fqn:" << fqn() << endl;
}

void SinkReactor::process_request (input_t<int>& req, output_t<int>& rsp) {
    cout << "(" << get_elapsed_logical_time() << ", " << get_microstep() << "), physical_time: " << get_elapsed_physical_time() << " " <<
    "Received input:" << *req.get() << " bank:" << bank_index << endl;
    rsp.set (*req.get());
}
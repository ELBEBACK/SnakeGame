#include "view.hpp"
#include "model.hpp"
#include "controller.hpp"

#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>

namespace snakes {

Controller::Controller(Model& model, View& view) : model_(model), 
                                                    view_(view), 
                                                    paused_(false), 
                                                    running_(true) {}

void Controller::run() {
    
    while (running_) {
        view_.renderAll();
        sleep(10);
    }
}

void Controller::processInput() {}

void Controller::pause()    { paused_ = true; }
void Controller::exit()     { running_ = false; }


}
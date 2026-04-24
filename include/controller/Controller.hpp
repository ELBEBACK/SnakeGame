#pragma once

#include "model/Model.hpp"
#include "view/View.hpp"
#include <random>

class Controller {
    Model& model_;
    View&  view_;

public:
    Controller(Model& model, View& view) : model_(model), view_(view) {}

    void run(int num_silly_bots, int num_smart_bots, int num_humans, int rounds);

private:
    void spawnSnakes(int num_silly_bots, int num_smart_bots, int num_humans);
};

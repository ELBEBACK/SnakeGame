#pragma once

#include "Model.hpp"



class View {
public:
    virtual ~View() = default;

    
    virtual Event getEvent(long tick_ms) = 0;

    
    virtual void render(Model& model) = 0;
};

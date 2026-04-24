#pragma once
#include <string>
#include <vector>

struct PlayerScore {
    int slot;
    std::string name;
    int color;
    int total_score;
};

using Scoreboard = std::vector<PlayerScore>;

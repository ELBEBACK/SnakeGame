#include "controller/Controller.hpp"
#include "controller/Scoreboard.hpp"

#include <unistd.h>
#include <chrono>
#include <iostream>
#include <optional>
#include <thread>



void Controller::spawnSnakes(int num_silly, int num_smart, int num_humans) {
    if (num_humans > 2) {
        std::cerr << "Warning: maximum 2 human players, clamping.\n";
        num_humans = 2;
    }

    const std::vector<Segment> human_starts = {
        Segment(model_.getWidth()  / 4,     model_.getHeight() * 3 / 4, SegmentType::HEAD),
        Segment(model_.getWidth()  * 3 / 4, model_.getHeight() * 3 / 4, SegmentType::HEAD),
    };

    const std::vector<int> colors = {32, 34, 33, 36, 35};
    int color_idx = 0;

    static std::mt19937 rng(
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())
    );

    auto rand_pos = [&]() -> std::optional<Segment> {
        std::uniform_int_distribution<int> dx(model_.getColBorder() + 1,
                                              model_.getWidth()  - model_.getColBorder());
        std::uniform_int_distribution<int> dy(model_.getRowBorder() + 1,
                                              model_.getHeight() - model_.getRowBorder());
        for (int i = 0; i < 200; ++i) {
            int x = dx(rng), y = dy(rng);
            if (model_.isPositionFree(x, y)) return Segment(x, y, SegmentType::HEAD);
        }
        return std::nullopt;
    };

    for (int i = 0; i < num_humans; ++i) {
        model_.addSnake(Snake::Builder()
            .setControlledBy(ControlledBy::HUMAN)
            .setDirection(Direction::UP)
            .setColor(colors[color_idx++ % colors.size()])
            .setBody(human_starts[i])
            .build());
    }

    for (int i = 0; i < num_silly; ++i) {
        auto pos = rand_pos();
        if (!pos) { std::cerr << "Warning: could not spawn silly bot " << i << "\n"; continue; }
        model_.addSnake(Snake::Builder()
            .setControlledBy(ControlledBy::SILLY_BOT)
            .setColor(colors[color_idx++ % colors.size()])
            .setBody(*pos)
            .build());
    }

    for (int i = 0; i < num_smart; ++i) {
        auto pos = rand_pos();
        if (!pos) { std::cerr << "Warning: could not spawn smart bot " << i << "\n"; continue; }
        model_.addSnake(Snake::Builder()
            .setControlledBy(ControlledBy::SMART_BOT)
            .setColor(colors[color_idx++ % colors.size()])
            .setBody(*pos)
            .build());
    }
}

void Controller::run(int num_silly, int num_smart, int num_humans, int rounds) {
    Scoreboard scoreboard;
    
    for (int r = 1; r <= rounds; ++r) {
        model_.reset();
        spawnSnakes(num_silly, num_smart, num_humans);

        bool paused = false;

        std::unordered_map<int, int> current_round_score;

        while (!model_.isOver()) {
            auto tick_start = std::chrono::steady_clock::now();
            long remaining  = static_cast<long>(model_.getTickMs());

            std::vector<Event> events;

            while (remaining > 0) {
                Event e = view_.getEvent(remaining);

                if (e.type == EventType::HALT)  return;
                if (e.type == EventType::PAUSE) paused = !paused;
                if (e.isValid())                events.push_back(e);

                remaining = static_cast<long>(model_.getTickMs()) -
                            std::chrono::duration_cast<std::chrono::milliseconds>(
                                std::chrono::steady_clock::now() - tick_start).count();
            }

            if (!paused) {
                model_.update(events);
                view_.render(model_);
                for (const auto& snake : model_.getSnakes()) {
                    if (snake.getState() == SnakeStatus::ALIVE) {
                        current_round_score[snake.getID()] = snake.getLength();
                    }
                }
            }
        }

        for (auto& ps : scoreboard) {
            ps.total_score += current_round_score[ps.slot];
        }

        view_.showScoreboard(scoreboard);

        auto end = std::chrono::steady_clock::now() + std::chrono::seconds(3);
        while (std::chrono::steady_clock::now() < end) {
            view_.render(model_);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            Event e = view_.getEvent(100);
            if (e.type == EventType::HALT) return;
        }
    }
}

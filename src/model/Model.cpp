#include "model/Model.hpp"

#include <queue>
#include <unordered_set>
#include <algorithm>
#include <climits>
#include <random>



void Model::rebuild_cells() {
    cells_.clear();
    for (const auto& snake : snakes_) {
        if (snake.getState() == SnakeStatus::ROTTED) continue;
        for (const auto& seg : snake.getBody()) {
            cells_[{seg.x, seg.y}] = CellType::SNAKE;
        }
    }
    for (const auto& gecko : geckos_) {
        cells_[{gecko.getX(), gecko.getY()}] = CellType::GECKO;
    }
}


CellType Model::getCell(int x, int y) const {
    auto it = cells_.find({x, y});
    return (it != cells_.end()) ? it->second : CellType::EMPTY;
}



void Model::addSnake(Snake snake) {
    snake.setID(next_id_++);
    if (snake.isHuman()) {
        human_ids_.push_back(snake.getID());
    }
    snakes_.push_back(std::move(snake));
}

bool Model::isPositionFree(int x, int y) const {
    if (x < minX() || x > maxX() || y < minY() || y > maxY()) return false;
    for (const auto& snake : snakes_) {
        for (const auto& seg : snake.getBody()) {
            if (seg.x == x && seg.y == y) return false;
        }
    }
    for (const auto& gecko : geckos_) {
        if (gecko.getX() == x && gecko.getY() == y) return false;
    }
    return true;
}

bool Model::checkBoundary(const Snake& snake) const {
    const Segment& head = snake.getHead();
    return head.x < minX() || head.x > maxX() ||
           head.y < minY() || head.y > maxY();
}

bool Model::checkSnakeCollision(const Snake& snake) const {
    const Segment& head = snake.getHead();
    for (const auto& other : snakes_) {
        if (other.getState() != SnakeStatus::ALIVE) continue;
        auto it = (&other == &snake)
                  ? std::next(other.getBody().begin())  
                  : other.getBody().begin();
        for (; it != other.getBody().end(); ++it) {
            if (it->x == head.x && it->y == head.y) return true;
        }
    }
    return false;
}

void Model::handleGeckoCollision(Snake& snake) {
    const Segment& head = snake.getHead();
    auto it = geckos_.begin();
    while (it != geckos_.end()) {
        if (it->getX() == head.x && it->getY() == head.y) {
            it = geckos_.erase(it);
            snake.grow();
        } else {
            ++it;
        }
    }
}

void Model::spawnGecko() {
    static std::mt19937 rng(
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count())
    );
    std::uniform_int_distribution<int> distX(minX(), maxX());
    std::uniform_int_distribution<int> distY(minY(), maxY());

    for (int attempt = 0; attempt < 100; ++attempt) {
        int x = distX(rng);
        int y = distY(rng);
        if (isPositionFree(x, y)) {
            geckos_.push_back(Gecko(x, y));
            return;
        }
    }
}

void Model::pruneDeadGeckos() {
    geckos_.remove_if([this](const Gecko& r) {
        return r.getX() < minX() || r.getX() > maxX() ||
               r.getY() < minY() || r.getY() > maxY();
    });
}

Gecko Model::nearestGecko(const Snake& snake) const {
    const Segment& head = snake.getHead();
    const Gecko* best  = nullptr;
    int           best_dist = INT_MAX;

    for (const auto& r : geckos_) {
        int d = std::abs(r.getX() - head.x) + std::abs(r.getY() - head.y);
        if (d < best_dist) { best_dist = d; best = &r; }
    }
    return best ? *best : Gecko(-1, -1);
}

void Model::applyPlayerDirection(int player_idx, Direction dir) {
    if (player_idx < 0 || player_idx >= static_cast<int>(human_ids_.size())) return;

    int id = human_ids_[player_idx];
    auto it = std::find_if(snakes_.begin(), snakes_.end(),
                           [id](const Snake& s){ return s.getID() == id; });

    if (it == snakes_.end() || it->getState() != SnakeStatus::ALIVE) return;

    Direction cur = it->getDirection();
    bool opposite = (dir == Direction::UP    && cur == Direction::DOWN)  ||
                    (dir == Direction::DOWN  && cur == Direction::UP)    ||
                    (dir == Direction::LEFT  && cur == Direction::RIGHT) ||
                    (dir == Direction::RIGHT && cur == Direction::LEFT);
    if (!opposite) it->setDirection(dir);
}

bool Model::isSafe(int x, int y) const {
    if (x < minX() || x > maxX() || y < minY() || y > maxY()) return false;
    auto it = cells_.find({x, y});
    return it == cells_.end() || it->second != CellType::SNAKE;
}

Direction Model::bfsSmartBot(const Snake& snake) const {
    const Segment& head    = snake.getHead();
    CellPoint      head_cp = {head.x, head.y};

    std::queue<CellPoint>                                    bfs_queue;
    std::unordered_map<CellPoint, CellPoint, CellPointHash> parents;
    std::unordered_set<CellPoint, CellPointHash>             visited;

    bfs_queue.push(head_cp);
    visited.insert(head_cp);

    const std::array<CellPoint, 4> deltas{{{0,-1},{0,1},{-1,0},{1,0}}};

    while (!bfs_queue.empty()) {
        CellPoint cur = bfs_queue.front();
        bfs_queue.pop();

        
        if (getCell(cur.x, cur.y) == CellType::GECKO) {
            CellPoint step = cur;
            while (!(parents.at(step) == head_cp)) {
                step = parents.at(step);
            }
            int dx = step.x - head.x;
            int dy = step.y - head.y;
            if (dx ==  1) return Direction::RIGHT;
            if (dx == -1) return Direction::LEFT;
            if (dy ==  1) return Direction::DOWN;
            return Direction::UP;
        }

        for (CellPoint delta : deltas) {
            CellPoint next = {cur.x + delta.x, cur.y + delta.y};
            if (visited.count(next)) continue;
            bool in_bounds = next.x >= minX() && next.x <= maxX() &&
                             next.y >= minY() && next.y <= maxY();
            if (in_bounds && getCell(next.x, next.y) != CellType::SNAKE) {
                bfs_queue.push(next);
                parents[next] = cur;
                visited.insert(next);
            }
        }
    }

    return snake.getDirection();  
}

Direction Model::greedyEasyBot(const Snake& snake) const {
    const Segment& head    = snake.getHead();
    Direction      cur_dir = snake.getDirection();
    Direction      desired = cur_dir;

    Gecko food = nearestGecko(snake);
    if (food.getX() != -1) {
        int fx = food.getX(), fy = food.getY();
        if      (fx > head.x && cur_dir != Direction::LEFT)  desired = Direction::RIGHT;
        else if (fx < head.x && cur_dir != Direction::RIGHT) desired = Direction::LEFT;
        else if (fy > head.y && cur_dir != Direction::UP)    desired = Direction::DOWN;
        else if (fy < head.y && cur_dir != Direction::DOWN)  desired = Direction::UP;
    }

    
    auto next_pos = [](int hx, int hy, Direction d) -> std::pair<int,int> {
        switch (d) {
            case Direction::UP:    return {hx, hy - 1};
            case Direction::DOWN:  return {hx, hy + 1};
            case Direction::LEFT:  return {hx - 1, hy};
            case Direction::RIGHT: return {hx + 1, hy};
        }
        return {hx, hy};
    };

    auto [nx, ny] = next_pos(head.x, head.y, desired);
    if (isSafe(nx, ny)) return desired;

    
    std::vector<Direction> fallbacks = {cur_dir};
    if (cur_dir == Direction::UP || cur_dir == Direction::DOWN) {
        fallbacks.push_back(Direction::LEFT);
        fallbacks.push_back(Direction::RIGHT);
    } else {
        fallbacks.push_back(Direction::UP);
        fallbacks.push_back(Direction::DOWN);
    }

    for (Direction d : fallbacks) {
        auto [fx, fy] = next_pos(head.x, head.y, d);
        if (isSafe(fx, fy)) return d;
    }

    return cur_dir;  
}

void Model::tickBots() {
    for (auto& snake : snakes_) {
        if (!snake.isBot() || snake.getState() != SnakeStatus::ALIVE) continue;

        Direction dir = (snake.getCtrl() == ControlledBy::SMART_BOT)
                        ? bfsSmartBot(snake)
                        : greedyEasyBot(snake);
        snake.setDirection(dir);
    }
}

void Model::update(const std::vector<Event>& events) {
    
    std::optional<Direction> dir1, dir2;
    for (const auto& e : events) {
        switch (e.type) {
            case EventType::UP_1:    dir1 = Direction::UP;    break;
            case EventType::DOWN_1:  dir1 = Direction::DOWN;  break;
            case EventType::LEFT_1:  dir1 = Direction::LEFT;  break;
            case EventType::RIGHT_1: dir1 = Direction::RIGHT; break;
            case EventType::UP_2:    dir2 = Direction::UP;    break;
            case EventType::DOWN_2:  dir2 = Direction::DOWN;  break;
            case EventType::LEFT_2:  dir2 = Direction::LEFT;  break;
            case EventType::RIGHT_2: dir2 = Direction::RIGHT; break;
            case EventType::HALT:    state_ = ModelState::GAME_OVER; return;
            default: break;
        }
    }
    if (dir1) applyPlayerDirection(0, *dir1);
    if (dir2) applyPlayerDirection(1, *dir2);

    
    rebuild_cells();
    tickBots();

    
    pruneDeadGeckos();

    
    for (auto& snake : snakes_) {
        if (snake.getState() != SnakeStatus::ALIVE) continue;

        snake.move();

        if (checkBoundary(snake) || checkSnakeCollision(snake)) {
            snake.kill();
        } else {
            handleGeckoCollision(snake);
        }
    }

    
    snakes_.remove_if([](const Snake& s){ return s.getState() == SnakeStatus::ROTTED; });

    
    for (auto& snake : snakes_) {
        if (snake.getState() == SnakeStatus::DEAD) snake.rot();
    }

    
    if (snakes_.empty()) {
        state_ = ModelState::GAME_OVER;
        return;
    }

    
    auto now     = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - gecko_timer_).count();
    if (elapsed >= SPAWN_INTERVAL_MS) {
        gecko_timer_ = now;
        if (static_cast<int>(geckos_.size()) < MAX_GECKOS) spawnGecko();
    }
}
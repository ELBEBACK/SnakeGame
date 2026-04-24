#pragma once

#include <list>
#include <vector>
#include <unordered_map>
#include <optional>
#include <chrono>

#include "model/Snake.hpp"
#include "model/Gecko.hpp"



constexpr long long SPAWN_INTERVAL_MS = 500;   
constexpr int       MAX_GECKOS        =   5;
constexpr int       BORDER_COL        =   2;   
constexpr int       BORDER_ROW        =   2;   

enum class EventType {
    
    UP_1, DOWN_1, LEFT_1, RIGHT_1,
    
    UP_2, DOWN_2, LEFT_2, RIGHT_2,

    RESIZE,   
    PAUSE,
    HALT,
    BAD,
};


struct Event {
    EventType type;

    Event()              : type(EventType::BAD)  {}
    Event(EventType t)   : type(t)               {}

    bool isValid() const { return type != EventType::BAD; }
};



enum class CellType { EMPTY, SNAKE, GECKO };

struct CellPoint {
    int x, y;
    bool operator==(const CellPoint& o) const { return x == o.x && y == o.y; }
};

struct CellPointHash {
    std::size_t operator()(const CellPoint& p) const {
        return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
    }
};



enum class ModelState { RUNNING, GAME_OVER };

class Model {
    ModelState          state_{ModelState::RUNNING};
    int                 width_;
    int                 height_;
    size_t              tick_ms_;          
    int                 next_id_{0};

    std::list<Snake>    snakes_;
    std::list<Gecko>    geckos_;
    std::vector<int>    human_ids_;        

    
    std::unordered_map<CellPoint, CellType, CellPointHash> cells_;

    
    std::chrono::steady_clock::time_point gecko_timer_{std::chrono::steady_clock::now()};

public:
    Model(int width, int height, size_t tick_ms)
        : width_(width), height_(height), tick_ms_(tick_ms) {}

    

    void update(const std::vector<Event>& events);

    

    bool      isOver()    const { return state_ == ModelState::GAME_OVER; }
    size_t    getTickMs() const { return tick_ms_; }
    int       getWidth()  const { return width_; }
    int       getHeight() const { return height_; }
    int       getColBorder() const { return BORDER_COL; }
    int       getRowBorder() const { return BORDER_ROW; }

    std::list<Snake>&        getSnakes()  { return snakes_; }
    const std::list<Snake>&  getSnakes()  const { return snakes_; }
    const std::list<Gecko>& getGeckos() const { return geckos_; }

    

    void setWidth(int w)  { width_  = w; }
    void setHeight(int h) { height_ = h; }
    void addSnake(Snake snake);
    bool isPositionFree(int x, int y) const;

private:
    
    int minX() const { return BORDER_COL + 1; }
    int maxX() const { return width_  - BORDER_COL; }
    int minY() const { return BORDER_ROW + 1; }
    int maxY() const { return height_ - BORDER_ROW; }

    
    void rebuild_cells();
    CellType getCell(int x, int y) const;

    
    bool checkBoundary(const Snake& snake) const;
    bool checkSnakeCollision(const Snake& snake) const;
    void handleGeckoCollision(Snake& snake);

    
    void spawnGecko();
    void pruneDeadGeckos();
    Gecko nearestGecko(const Snake& snake) const;

    
    void applyPlayerDirection(int player_idx, Direction dir);

    
    void tickBots();
    Direction bfsSmartBot(const Snake& snake) const;   
    Direction greedyEasyBot(const Snake& snake) const; 

    bool isSafe(int x, int y) const;
};

#pragma once

#include <list>



enum class Direction    { UP, DOWN, LEFT, RIGHT };
enum class SegmentType  { HEAD, BODY };
enum class SnakeStatus  { ALIVE, DEAD, ROTTED };
enum class ControlledBy { HUMAN, SMART_BOT, SILLY_BOT };



struct Segment {
    int         x, y;
    SegmentType type;

    Segment(int x, int y, SegmentType t = SegmentType::BODY) : x(x), y(y), type(t) {}
    Segment() : x(0), y(0), type(SegmentType::BODY) {}
};



class Snake {
    int                id_{-1};        
    ControlledBy       ctrl_;
    SnakeStatus        state_{SnakeStatus::ALIVE};
    std::list<Segment> body_;
    Direction          direction_;
    int                color_;
    Segment            rudimentary_tail_{};  
    bool               should_grow_{false};
public:
    Snake(ControlledBy ctrl, std::list<Segment> body, Direction dir, int color)
        : ctrl_(ctrl), body_(std::move(body)), direction_(dir), color_(color) {}

    
    struct Builder {
        ControlledBy       ctrl_ {ControlledBy::SILLY_BOT};
        std::list<Segment> body_ {};
        Direction          dir_  {Direction::RIGHT};
        int                color_{37};  

        Builder& setControlledBy(ControlledBy c)  { ctrl_  = c; return *this; }
        Builder& setDirection(Direction d)        { dir_   = d; return *this; }
        Builder& setColor(int c)                  { color_ = c; return *this; }

        
        
        Builder& setBody(Segment pos) {
            body_.push_back(Segment(pos.x, pos.y, SegmentType::HEAD));
            body_.push_back(Segment(pos.x, pos.y, SegmentType::BODY));
            body_.push_back(Segment(pos.x, pos.y, SegmentType::BODY));
            return *this;
        }

        Snake build() const { return Snake(ctrl_, body_, dir_, color_); }
    };

    
    void move();
    void grow()              { should_grow_ = true; }
    void kill()              { state_ = SnakeStatus::DEAD; }
    void rot()               { state_ = SnakeStatus::ROTTED; }
    void setID(int id)       { id_ = id; }
    void setDirection(Direction d) { direction_ = d; }

    
    int                        getID()        const noexcept { return id_; }
    SnakeStatus                getState()     const noexcept { return state_; }
    Direction                  getDirection() const noexcept { return direction_; }
    int                        getColor()     const noexcept { return color_; }
    int                        getLength()    const noexcept { return static_cast<int>(body_.size()); }
    const std::list<Segment>&  getBody()      const noexcept { return body_; }
    Segment                    getHead()      const noexcept { return body_.front(); }
    Segment                    getTail()      const noexcept { return rudimentary_tail_; }
    ControlledBy               getCtrl()      const noexcept { return ctrl_; }

    bool isHuman() const noexcept { return ctrl_ == ControlledBy::HUMAN; }
    bool isBot()   const noexcept { return ctrl_ != ControlledBy::HUMAN; }
};

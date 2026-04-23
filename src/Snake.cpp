#include "Snake.hpp"



void Snake::move() {
    if (body_.empty()) return;

    
    Segment new_head       = body_.front();
    new_head.type          = SegmentType::HEAD;
    body_.front().type     = SegmentType::BODY;

    switch (direction_) {
        case Direction::UP:    new_head.y--; break;
        case Direction::DOWN:  new_head.y++; break;
        case Direction::LEFT:  new_head.x--; break;
        case Direction::RIGHT: new_head.x++; break;
    }

    body_.push_front(new_head);

    if (should_grow_) {
        should_grow_ = false;
        
    } else {
        rudimentary_tail_ = body_.back();
        body_.pop_back();
    }
}
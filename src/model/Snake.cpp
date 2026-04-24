#include "model/Snake.hpp"



static Direction direction_between(const Segment& from, const Segment& to) {
    int dx = to.x - from.x;
    int dy = to.y - from.y;
    if (dx > 0) return Direction::RIGHT;
    if (dx < 0) return Direction::LEFT;
    if (dy > 0) return Direction::DOWN;
    if (dy < 0) return Direction::UP;
    return Direction::RIGHT;
}


void Snake::rebuild_directions() {
    if (body_.size() < 2) return;
 
    auto curr = body_.begin();
    auto next = std::next(curr);
 
    while (next != body_.end()) {
        Direction toward_tail = direction_between(*curr, *next);
        Direction toward_head = direction_between(*next, *curr);
 
        curr->direction_tail = toward_tail;
        next->direction_head = toward_head;
 
        curr = next;
        ++next;
    }
 
    body_.front().direction_head = opposite(body_.front().direction_tail);
    body_.back().direction_tail  = opposite(body_.back().direction_head);
}


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

    rebuild_directions();
}
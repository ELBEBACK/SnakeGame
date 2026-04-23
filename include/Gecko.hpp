#pragma once

class Gecko {
    int x_;
    int y_;

public:
    Gecko(int x, int y) : x_(x), y_(y) {}

    void setPosition(int x, int y) { x_ = x; y_ = y; }

    int getX() const noexcept { return x_; }
    int getY() const noexcept { return y_; }
};

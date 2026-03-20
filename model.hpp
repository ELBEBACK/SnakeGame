#ifndef SNAKES_MODEL_HEADER
#define SNAKES_MODEL_HEADER

namespace snakes {

enum Direction { LEFT, UP, RIGHT, DOWN };

struct Position {
    int x, y;
};

class Model {

    unsigned short width_;
    unsigned short height_;
    //Snake snake_;
    //Rabbit rabbit_;
    unsigned int score_;
    Direction currentD_;
    bool gameover_;

public:
    Model(unsigned short width, unsigned short height) : width_(width), height_(height) {}

public:

    void update() {}

    bool isGameover() const { return true; }

    void setDirection() {}
    Direction getDirection() const { return UP; }

private:
    bool checkInterception() const { return true; }
    void growSnake() {}

};

}

#endif